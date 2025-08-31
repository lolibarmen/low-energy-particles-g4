#ifndef STEPPING_ACTION_HPP
#define STEPPING_ACTION_HPP

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "RunAction.hpp"
#include "DetectorConstruction.hpp"

class SteppingAction : public G4UserSteppingAction {
public:
    SteppingAction(RunAction* runAction, DetectorConstruction* detConstruction)
        : runAction(runAction), detConstruction(detConstruction) {}
    
    virtual ~SteppingAction() {}
    
    virtual void UserSteppingAction(const G4Step* step) override {
        // Получаем энергетические депозиты
        G4double energyDeposit = step->GetTotalEnergyDeposit();
        if (energyDeposit <= 0.0) return;
        
        // Получаем длину шага
        G4double stepLength = step->GetStepLength();
        
        // Получаем объем и проверяем, что это фантом
        G4LogicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
        G4String volumeName = volume->GetName();
        
        if (volumeName == "Phantom") {
            // Передаем энергию и длину трека в RunAction
            runAction->AddEnergyDeposition(energyDeposit);
            runAction->AddTrackLength(stepLength);
            
            // Вычисляем глубину в фантоме
            G4double depth = CalculateDepthInPhantom(step);
            
            // Заполняем гистограмму распределения дозы по глубине
            runAction->FillDoseDepthProfile(depth, energyDeposit);
            
            // Собираем дополнительную информацию о первичных частицах
            CollectPrimaryParticleInfo(step);
        }
        
        // Дополнительная информация для отладки (редкие события)
        if (energyDeposit > 1*MeV && step->GetTrack()->GetCurrentStepNumber() == 1) {
            G4cout << "Large energy deposit: " << G4BestUnit(energyDeposit, "Energy")
                   << " by " << step->GetTrack()->GetParticleDefinition()->GetParticleName()
                   << " at step " << step->GetTrack()->GetCurrentStepNumber() << G4endl;
        }
    }
    
    G4double CalculateDepthInPhantom(const G4Step* step) {
        // Получаем позицию шага
        G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
        
        // Получаем размеры фантома
        G4ThreeVector phantomSize = detConstruction->GetPhantomSize();
        G4double phantomStartZ = -phantomSize.z() / 2.0;
        
        // Вычисляем глубину от поверхности фантома
        G4double depth = position.z() - phantomStartZ;
        
        // Обеспечиваем корректные границы
        depth = std::max(0.0, std::min(depth, phantomSize.z()));
        
        return depth;
    }
    
    void CollectPrimaryParticleInfo(const G4Step* step) {
        // Собираем информацию только о первичных частицах
        G4Track* track = step->GetTrack();
        if (track->GetParentID() == 0 && track->GetCurrentStepNumber() == 1) {
            // Первый шаг первичной частицы
            G4double primaryEnergy = track->GetKineticEnergy();
            runAction->FillParticleEnergy(primaryEnergy);
            
            if (track->GetTrackID() % 1000 == 1) {
                G4cout << "Primary " << track->GetParticleDefinition()->GetParticleName()
                       << " energy: " << G4BestUnit(primaryEnergy, "Energy") << G4endl;
            }
        }
    }
    
    void PrintStepInfo(const G4Step* step) {
        // Функция для отладки - печать информации о шаге
        G4Track* track = step->GetTrack();
        G4cout << "Step #" << track->GetCurrentStepNumber()
               << " Particle: " << track->GetParticleDefinition()->GetParticleName()
               << " E_dep: " << G4BestUnit(step->GetTotalEnergyDeposit(), "Energy")
               << " Step length: " << G4BestUnit(step->GetStepLength(), "Length")
               << " Volume: " << step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetName()
               << G4endl;
    }

private:
    RunAction* runAction;
    DetectorConstruction* detConstruction;
};

#endif // STEPPING_ACTION_HPP