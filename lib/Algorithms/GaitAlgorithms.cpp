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

void GaitAnalyzer::reset()
{
    leftStepCount_ = rightStepCount_ = 0;
    leftAccumulator_ = rightAccumulator_ = 0.0f;

    // Réinitialisation des buffers de course
    leftBufferIdx_ = rightBufferIdx_ = 0;
    leftBufferCount_ = rightBufferCount_ = 0;
    for (uint8_t i = 0; i < BUFFER_SIZE; ++i)
    {
        leftBuffer_[i] = 0.0f;
        rightBuffer_[i] = 0.0f;
    }
}

bool GaitAnalyzer::addCalibrationStep(float force, bool isLeft)
{
    if (isLeft && leftStepCount_ < 16)
    {
        leftAccumulator_ += force;
        leftStepCount_++;
    }
    else if (!isLeft && rightStepCount_ < 16)
    {
        rightAccumulator_ += force;
        rightStepCount_++;
    }
    return (leftStepCount_ >= 16 && rightStepCount_ >= 16);
}

// Enregistre un pas en mode course dans le filtre de moyenne mobile
void GaitAnalyzer::addRunningStep(float force, bool isLeft)
{
    if (isLeft)
    {
        leftBuffer_[leftBufferIdx_] = force;
        leftBufferIdx_ = (leftBufferIdx_ + 1) % BUFFER_SIZE;
        if (leftBufferCount_ < BUFFER_SIZE)
            leftBufferCount_++;
    }
    else
    {
        rightBuffer_[rightBufferIdx_] = force;
        rightBufferIdx_ = (rightBufferIdx_ + 1) % BUFFER_SIZE;
        if (rightBufferCount_ < BUFFER_SIZE)
            rightBufferCount_++;
    }
}

float GaitAnalyzer::getLeftAverage() const
{
    if (leftBufferCount_ == 0)
        return 0.0f;
    float sum = 0.0f;
    for (uint8_t i = 0; i < leftBufferCount_; ++i)
        sum += leftBuffer_[i];
    return sum / leftBufferCount_;
}

float GaitAnalyzer::getRightAverage() const
{
    if (rightBufferCount_ == 0)
        return 0.0f;
    float sum = 0.0f;
    for (uint8_t i = 0; i < rightBufferCount_; ++i)
        sum += rightBuffer_[i];
    return sum / rightBufferCount_;
}

float GaitAnalyzer::computeAsymmetry(float left, float right) const
{
    if (left < minForceThreshold_ || right < minForceThreshold_)
        return 0.0f;
    return (std::abs(left - right) / std::max(left, right)) * 100.0f;
}