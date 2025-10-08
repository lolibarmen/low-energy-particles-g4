#ifndef RUN_ACTION_HPP
#define RUN_ACTION_HPP

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4AnalysisManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "DetectorConstruction.hpp"

class RunAction : public G4UserRunAction {
public:
    RunAction(DetectorConstruction* detConstruction) 
        : detConstruction(detConstruction), 
          totalEnergyDeposited(0.0), 
          totalTrackLength(0.0) {}
    
    virtual ~RunAction() {}
    
    virtual void BeginOfRunAction(const G4Run* run) override {
        // Инициализация анализатора
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
        analysisManager->SetDefaultFileType("root");
        analysisManager->OpenFile("/tmp/dose_analysis.root");
        
        // Создаем гистограммы для распределения дозы по глубине
        G4double phantomDepth = detConstruction->GetPhantomSize().z();
        analysisManager->CreateH1("dose_depth", "Dose distribution along depth", 
                                 200, 0, 5*cm, "cm", "keV");
        analysisManager->CreateH1("energy_deposition", "Energy deposition per event", 
                                 100, 0, 1*MeV, "MeV");
        analysisManager->CreateH1("particle_energy", "Primary particle energy spectrum", 
                                 100, 0, 20*MeV, "MeV");
        
        // Сбрасываем счетчики
        totalEnergyDeposited = 0.0;
        totalTrackLength = 0.0;
        
        G4cout << "### Run " << run->GetRunID() << " started." << G4endl;
    }
    
    virtual void EndOfRunAction(const G4Run* run) override {
        // Получаем анализ manager
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
        
        // Нормализуем график распределения
        G4int numEvents = run->GetNumberOfEvent();
        analysisManager->ScaleH1(0, 1.0 / numEvents);

        // Сохраняем и закрываем файл
        analysisManager->Write();
        analysisManager->CloseFile();
        
        // Выводим статистику
        G4double averageEnergyDeposited = totalEnergyDeposited / numEvents;
        G4double averageTrackLength = totalTrackLength / numEvents;
        
        G4cout << "\n\n=== Run Summary ===" << G4endl;
        G4cout << "Number of events: " << numEvents << G4endl;
        G4cout << "Total energy deposited: " << G4BestUnit(totalEnergyDeposited, "Energy") << G4endl;
        G4cout << "Average energy deposited per event: " << G4BestUnit(averageEnergyDeposited, "Energy") << G4endl;
        G4cout << "Average track length per event: " << G4BestUnit(averageTrackLength, "Length") << G4endl;
        G4cout << "Output file: dose_analysis.root" << G4endl;
        G4cout << "==================\n\n" << G4endl;
    }
    
    void AddEnergyDeposition(G4double energy) {
        totalEnergyDeposited += energy;
    }
    
    void AddTrackLength(G4double length) {
        totalTrackLength += length;
    }
    
    void FillDoseDepthProfile(G4double depth, G4double energy) {
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
        analysisManager->FillH1(0, depth, energy);
    }
    
    void FillEnergyDeposition(G4double energy) {
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
        analysisManager->FillH1(1, energy);
    }
    
    void FillParticleEnergy(G4double energy) {
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
        analysisManager->FillH1(2, energy);
    }

private:
    DetectorConstruction* detConstruction;
    G4double totalEnergyDeposited;
    G4double totalTrackLength;
};

#endif // RUN_ACTION_HPP