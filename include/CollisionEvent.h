// include/CollisionEvent.h
#ifndef COLLISION_EVENT_H
#define COLLISION_EVENT_H

#include <string>

/**
 * @struct CollisionEvent
 * @brief Represents a proton-proton collision event from ATLAS data
 *
 * Stores key event attributes for energy conservation analysis.
 *
 * Background: In LHC (Large Hadron Collider) collisions, two 6.5 TeV protons collide, producing particles
 * via QCD (Quantum Chromodyanmics) processes. Rest-energy out is the sum of rest masses of detected particles, a small
 * fraction of the 13 TeV total due to kinetic energy dominance in final states.
 */

struct CollisionEvent {
    int eventId;                    ///< Unique identifies for the event
    std::string incomingParticles;  ///< Always "proton,proton" (max 32 chars)
    std::string outgoingParticles;  ///< List of detected particles (max 256 chars)
    float kineticEnergyIn;          ///< 13,000 GeV, LHC center-of-mass energy
    float restEnergyOut;            ///< Sum of rest masses in GeV
    float efficiency;               ///< restEnergyOut / kineticEnergyIn, conversion efficiency
};

#endif // COLLISION_EVENT_H