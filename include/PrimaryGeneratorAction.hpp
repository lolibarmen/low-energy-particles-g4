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
    PrimaryGeneratorAction() 
        : particleGun(new G4ParticleGun(1)),
          beamType("electron"),
          beamEnergy(6*MeV),
          beamSigma(2*mm),
          beamPosition(0, 0, -15*cm),
          beamDirection(0, 0, 1),
          useGaussianBeam(false),
          useDivergentBeam(false),
          beamDivergence(0.0) {
        
        ConfigureParticleGun();
    }
    
    virtual ~PrimaryGeneratorAction() {
        delete particleGun;
    }
    
    virtual void GeneratePrimaries(G4Event* event) override {
        // Устанавливаем энергию частицы
        particleGun->SetParticleEnergy(beamEnergy);
        
        // Устанавливаем позицию (возможна гауссова дистрибуция)
        G4ThreePosition position = beamPosition;
        if (useGaussianBeam) {
            position.setX(G4RandGauss::shoot(beamPosition.x(), beamSigma));
            position.setY(G4RandGauss::shoot(beamPosition.y(), beamSigma));
        }
        particleGun->SetParticlePosition(position);
        
        // Устанавливаем направление (возможна дивергенция)
        G4ThreeVector direction = beamDirection;
        if (useDivergentBeam && beamDivergence > 0.0) {
            G4double theta = G4RandGauss::shoot(0.0, beamDivergence);
            G4double phi = G4UniformRand() * 360.0 * deg;
            direction.setRThetaPhi(1.0, theta, phi);
        }
        particleGun->SetParticleMomentumDirection(direction);
        
        // Генерируем первичные частицы
        particleGun->GeneratePrimaryVertex(event);
        
        if (event->GetEventID() % 10000 == 0) {
            G4cout << "Generated event " << event->GetEventID() 
                   << " with " << beamType << " beam at " 
                   << beamEnergy/MeV << " MeV" << G4endl;
        }
    }
    
    void SetBeamType(const G4String& type) {
        beamType = type;
        ConfigureParticleGun();
    }
    
    void SetBeamEnergy(G4double energy) { 
        beamEnergy = energy; 
    }
    
    void SetBeamPosition(const G4ThreeVector& pos) { 
        beamPosition = pos; 
    }
    
    void SetBeamDirection(const G4ThreeVector& dir) { 
        beamDirection = dir.unit(); 
    }
    
    void SetBeamSigma(G4double sigma) { 
        beamSigma = sigma; 
        useGaussianBeam = (sigma > 0.0);
    }
    
    void SetBeamDivergence(G4double divergence) { 
        beamDivergence = divergence; 
        useDivergentBeam = (divergence > 0.0);
    }
    
    void SetParticleCount(G4int count) { 
        particleGun->SetNumberOfParticles(count); 
    }
    
    G4String GetBeamType() const { return beamType; }
    G4double GetBeamEnergy() const { return beamEnergy; }
    G4ThreeVector GetBeamPosition() const { return beamPosition; }
    G4ThreeVector GetBeamDirection() const { return beamDirection; }
    G4double GetBeamSigma() const { return beamSigma; }
    G4double GetBeamDivergence() const { return beamDivergence; }
    G4int GetParticleCount() const { return particleGun->GetNumberOfParticles(); }

private:
    void ConfigureParticleGun() {
        G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
        G4ParticleDefinition* particle = nullptr;
        
        if (beamType == "electron") {
            particle = particleTable->FindParticle("e-");
        } else if (beamType == "photon") {
            particle = particleTable->FindParticle("gamma");
        } else if (beamType == "positron") {
            particle = particleTable->FindParticle("e+");
        } else if (beamType == "proton") {
            particle = particleTable->FindParticle("proton");
        } else {
            // По умолчанию используем электроны
            particle = particleTable->FindParticle("e-");
            beamType = "electron";
        }
        
        if (particle) {
            particleGun->SetParticleDefinition(particle);
            G4cout << "Particle gun configured for: " << particle->GetParticleName() << G4endl;
        } else {
            G4Exception("PrimaryGeneratorAction::ConfigureParticleGun", 
                       "InvalidParticle", FatalException, 
                       "Particle type not found in particle table");
        }
    }
    
    void ConfigureElectronBeamParameters() {
        // Параметры типичного медицинского ускорителя электронов
        SetBeamSigma(2*mm);          // Размер пучка
        SetBeamDivergence(2.0*deg);  // Дивергенция пучка
        SetBeamPosition(G4ThreeVector(0, 0, -15*cm)); // Положение источника
    }
    
    void ConfigurePhotonBeamParameters() {
        // Параметры типичного фотонного пучка
        SetBeamSigma(3*mm);          // Размер пучка
        SetBeamDivergence(1.5*deg);  // Дивергенция пучка
        SetBeamPosition(G4ThreeVector(0, 0, -20*cm)); // Положение источника
    }

private:
    G4ParticleGun* particleGun;
    G4String beamType;
    G4double beamEnergy;
    G4double beamSigma;
    G4ThreeVector beamPosition;
    G4ThreeVector beamDirection;
    G4bool useGaussianBeam;
    G4bool useDivergentBeam;
    G4double beamDivergence;
};

#endif // PRIMARY_GENERATOR_ACTION_HPP