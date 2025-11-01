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

        // Согласно данным усорителя
        G4double radius = 6.0 * cm;

        // Данные для энергетического спектра
        const std::vector<G4double> energies = {0.1, 0.2, 0.30, 0.35, 0.40, 0.45, 0.50};
        const std::vector<G4double> probabilities = {0.0, 0.0, 0.0, 0.025, 0.05, 0.125, 0.8};
        
        // Генерация случайной энергии согласно распределению
        G4double randomValue = G4UniformRand();
        G4double cumulativeProb = 0.0;
        G4double selectedEnergy = energies.back(); // значение по умолчанию
        
        for (size_t i = 0; i < probabilities.size(); ++i) {
            cumulativeProb += probabilities[i];
            if (randomValue <= cumulativeProb) {
                selectedEnergy = energies[i];
                break;
            }
        }
        
        // Устанавливаем энергию частицы
        particleGun->SetParticleEnergy(selectedEnergy * MeV);

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

        // Генерация случайной точки в круге радиусом 10 см
        G4double r = radius * std::sqrt(G4UniformRand());  // равномерное распределение по площади
        G4double theta = 2 * M_PI * G4UniformRand();       // случайный угол

        G4double u = r * std::cos(theta);
        G4double v = r * std::sin(theta);

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