#ifndef SENSITIVE_DETECTOR_HPP
#define SENSITIVE_DETECTOR_HPP

#include "G4VSensitiveDetector.hh"
#include "G4AnalysisManager.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"

class SensitiveDetector : public G4VSensitiveDetector {
public:
    SensitiveDetector(const G4String& name) : G4VSensitiveDetector(name) {}
    virtual ~SensitiveDetector() {}
    
    virtual void Initialize(G4HCofThisEvent* hce) override {
        // Можно добавить инициализацию при необходимости
    }
    
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override {
        // Получаем энергию, депонированную на этом шаге
        G4double edep = step->GetTotalEnergyDeposit();
        if (edep == 0.) return false;  // Пропускаем шаги без депозита энергии
        
        // Получаем позицию депозита энергии
        G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
        
        // Получаем Analysis Manager
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
        
        // Заполняем гистограммы
        analysisManager->FillH1(0, position.z(), edep * keV);
        analysisManager->FillH1(1, edep);
        
        return true;
    }
    
    virtual void EndOfEvent(G4HCofThisEvent* hce) override {
        // Можно добавить обработку конца события
    }
};

#endif // SENSITIVE_DETECTOR_HPP