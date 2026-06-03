#pragma once
#include <optional>
#include <cstdint>

/**
 * @class ImpactDetector
 * @brief Détecte et isole le pic maximal d'accélération lors d'un impact au sol (un pas).
 */
class ImpactDetector
{
private:
    float thresholdG_;             // Seuil d'accélération (en G) au-dessus duquel on considère qu'un impact commence.
    bool isInsideImpact_ = false;  // Flag indiquant si le capteur est actuellement au milieu d'un impact (au-dessus du seuil).
    float currentPeak_ = 0.0f;     // Stocke la valeur maximale (le pic) enregistrée pendant l'impact en cours.
    uint32_t lastImpactEndMs_ = 0; // Horodatage (en ms) de la fin du dernier impact pour gérer le temps de refroidissement (cooldown).

public:
    // Constructeur : initialise le seuil de détection en G (ex: 2.0G).
    ImpactDetector(float thresholdG) : thresholdG_(thresholdG) {}

    // Traite chaque échantillon d'accélération reçu en temps réel.
    std::optional<float> processSample(float currentSample, uint32_t nowMs);

    void setThreshold(float newThreshold) { thresholdG_ = newThreshold; }
    float getThreshold() const { return thresholdG_; }
};

/**
 * @class GaitAnalyzer
 * @brief Gère la phase de calibration (accumulation des pas) et calcule l'asymétrie de la marche.
 */
class GaitAnalyzer
{
private:
    uint8_t leftStepCount_ = 0, rightStepCount_ = 0;         // Compteurs du nombre de pas valides pour chaque jambe.
    float leftAccumulator_ = 0.0f, rightAccumulator_ = 0.0f; // Somme des forces d'impact accumulées pendant la calibration.
    float minForceThreshold_ = 3.0f;                         // Seuil minimal de force pour qu'un pas soit pris en compte dans la calibration (en G).

public:
    // Réinitialise les compteurs et les accumulateurs à zéro.
    void reset();

    // Ajoute un pas à la phase de calibration. Renvoie true dès que la calibration est terminée.
    bool addCalibrationStep(float force, bool isLeft);

    // Calcule le pourcentage d'asymétrie entre le côté gauche et le côté droit.
    float computeAsymmetry(float leftPeak, float rightPeak) const;

    // Renvoie le nombre total de pas validés (gauche + droit).
    uint8_t getTotalSteps() const { return leftStepCount_ + rightStepCount_; }

    void setMinForceThreshold(float threshold) { minForceThreshold_ = threshold; }
    float getMinForceThreshold() const { return minForceThreshold_; }
};