#ifndef PRIMARY_GENERATOR_ACTION_HPP
#define PRIMARY_GENERATOR_ACTION_HPP

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Event.hh"
#include "Randomize.hh"
#include "G4ios.hh"

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction() : particleGun(new G4ParticleGun(1))
    {    
        ConfigureParticleGun();
    }
    
    virtual ~PrimaryGeneratorAction() {
        delete particleGun;
    }
    
    virtual void GeneratePrimaries(G4Event* anEvent) override {        
        G4ThreeVector basePosition = particleGun->GetParticlePosition();
        G4ThreeVector baseDirection = particleGun->GetParticleMomentumDirection();

        G4double planeSizeX = 10.0 * cm;
        G4double planeSizeY = 10.0 * cm;

        G4ThreeVector normal = baseDirection.unit();
        
        G4ThreeVector axisX, axisY;

        G4ThreeVector temp;
        if (std::abs(normal.z()) < 0.9) {
            temp = G4ThreeVector(0., 0., 1.);
        } else {
            temp = G4ThreeVector(1., 0., 0.);
        }

        axisX = (normal.cross(temp)).unit();
        axisY = (normal.cross(axisX)).unit();

        G4double u = (G4UniformRand() - 0.5) * planeSizeX;
        G4double v = (G4UniformRand() - 0.5) * planeSizeY;

        G4ThreeVector finalPosition = basePosition + u * axisX + v * axisY;
        
        particleGun->SetParticlePosition(finalPosition);
        
        particleGun->GeneratePrimaryVertex(anEvent);

        particleGun->SetParticlePosition(basePosition);
    }

private:
    void ConfigureParticleGun() {
        G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
        G4ParticleDefinition* particle = nullptr;

        particle = particleTable->FindParticle("e-");

        particleGun->SetParticleDefinition(particle);
        G4cout << "Particle gun configured for: " << particle->GetParticleName() << G4endl;
    }

private:
    G4ParticleGun* particleGun;
};

#endif // PRIMARY_GENERATOR_ACTION_HPP