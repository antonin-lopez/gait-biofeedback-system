#include "GaitAlgorithms.h"
#include <algorithm>
#include <cmath>

/**
 * @brief Analyse l'accélération actuelle pour détecter un pas et isoler sa force maximale.
 * @return std::nullopt si le pas est en cours ou en cooldown, sinon retourne la force du pic (float).
 */
std::optional<float> ImpactDetector::processSample(float currentSample, uint32_t nowMs)
{
    // SÉCURITÉ (Cooldown) : Si l'impact précédent s'est terminé il y a moins de 250 ms,
    // on ignore l'échantillon pour éviter de détecter deux fois le même pas (rebonds).
    if (!isInsideImpact_ && (nowMs - lastImpactEndMs_) < 250)
        return std::nullopt;

    // ÉTAPE 1 : Si on n'est pas encore dans un impact
    if (!isInsideImpact_)
    {
        // Si l'accélération dépasse le seuil configuré (ex: 2.0G), l'impact commence !
        if (currentSample > thresholdG_)
        {
            isInsideImpact_ = true;       // On passe à l'état "en cours d'impact"
            currentPeak_ = currentSample; // On initialise le pic avec la valeur actuelle
        }
    }
    // ÉTAPE 2 : Si on est déjà au milieu d'un impact
    else
    {
        // On met à jour le pic maximal : on garde la plus grande valeur entre l'ancien pic et l'échantillon actuel
        currentPeak_ = std::max(currentPeak_, currentSample);

        // Si l'accélération redescend en dessous du seuil, cela signifie que le pas est terminé
        if (currentSample <= thresholdG_)
        {
            isInsideImpact_ = false;  // L'impact est fini
            lastImpactEndMs_ = nowMs; // On enregistre l'heure de fin pour le cooldown
            return currentPeak_;      // On RENVOIE le pic maximal mesuré pendant ce pas !
        }
    }

    // Renvoie rien tant que le pas n'est pas complètement terminé et validé
    return std::nullopt;
}

/**
 * @brief Remet à zéro les données d'analyse (compteurs et sommes).
 */
void GaitAnalyzer::reset()
{
    leftStepCount_ = rightStepCount_ = 0;
    leftAccumulator_ = rightAccumulator_ = 0.0f;
}

/**
 * @brief Enregistre un pas pendant la calibration.
 * @param force Force maximale de l'impact reçu.
 * @param isLeft True si l'impact vient de la cheville gauche, False si droite.
 * @return true si la calibration est terminée (16 pas de chaque côté = 32 pas au total), sinon false.
 */
bool GaitAnalyzer::addCalibrationStep(float force, bool isLeft)
{
    // Si c'est le pied gauche et qu'on n'a pas encore atteint les 16 pas requis
    if (isLeft && leftStepCount_ < 16)
    {
        leftAccumulator_ += force; // On ajoute la force à la somme totale gauche
        leftStepCount_++;          // On incrémente le compteur de pas gauches
    }
    // Si c'est le pied droit et qu'on n'a pas encore atteint les 16 pas requis
    else if (!isLeft && rightStepCount_ < 16)
    {
        rightAccumulator_ += force; // On ajoute la force à la somme totale droite
        rightStepCount_++;          // On incrémente le compteur de pas droits
    }

    // Retourne TRUE uniquement quand on a validé exactement 16 pas à gauche ET 16 pas à droite.
    return (leftStepCount_ >= 16 && rightStepCount_ >= 16);
}

/**
 * @brief Calcule le pourcentage d'asymétrie entre le pied gauche et le pied droit.
 * @formula  (|Gauche - Droite| / Max(Gauche, Droite)) * 100
 */
float GaitAnalyzer::computeAsymmetry(float left, float right) const
{
    // Sécurité : Si l'une des forces est inférieure à minForceThreshold_, on considère que ce n'est pas un pas
    // de course valide (ex: piétinement à l'arrêt), donc on retourne 0% d'asymétrie.
    if (left < minForceThreshold_ || right < minForceThreshold_)
        return 0.0f;

    // Calcul mathématique de la différence relative par rapport à la force maximale exercée
    return (std::abs(left - right) / std::max(left, right)) * 100.0f;
}