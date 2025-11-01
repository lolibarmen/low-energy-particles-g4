#ifndef DETECTOR_CONSTRUCTION_HPP
#define DETECTOR_CONSTRUCTION_HPP

#include "G4VUserDetectorConstruction.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"

#include "SensitiveDetector.hpp"

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction() 
        : worldSize(50*cm), 
          phantomSize(30*cm, 30*cm, 20*cm),
          absorberThickness(5*mm),
          useAbsorber(true),
          absorberMaterial(nullptr),
          phantomMaterial(nullptr) {}
    
    virtual ~DetectorConstruction() {}
    
    virtual G4VPhysicalVolume* Construct() override {
        // Получаем менеджер материалов NIST
        G4NistManager* nist = G4NistManager::Instance();
        
        // Создаем материалы
        G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
        /*
        G4_WATER - вода
        G4_POLYETHYLENE - полиэтилен
        */
        phantomMaterial = nist->FindOrBuildMaterial("G4_Al");  // Вода как фантом тканей
        absorberMaterial = nist->FindOrBuildMaterial("G4_Pb");    // Свинец как поглотитель
        
        // Создаем мировой объем
        G4Box* worldSolid = new G4Box("World", worldSize/2, worldSize/2, worldSize/2);
        G4LogicalVolume* worldLogical = new G4LogicalVolume(worldSolid, air, "World");
        G4VPhysicalVolume* worldPhysical = new G4PVPlacement(0, G4ThreeVector(), worldLogical, 
                                                           "World", 0, false, 0);
        
        // Создаем фантом (мишень)
        G4Box* phantomSolid = new G4Box("Phantom", phantomSize.x()/2, phantomSize.y()/2, phantomSize.z()/2);
        phantomLogical = new G4LogicalVolume(phantomSolid, phantomMaterial, "Phantom");
        new G4PVPlacement(0, G4ThreeVector(0, 0, 0), phantomLogical, "Phantom", worldLogical, false, 0);

        G4double phantomVolume = phantomSolid->GetCubicVolume() / (m3);  // объем в мм³
        G4double phantomDensity = phantomMaterial->GetDensity() / (kg/m3);  // плотность в кг/м³

        phantomMass = phantomVolume * phantomDensity;  // масса в кг

        printf("Phantom mass: %fkg\n", (float)phantomMass);
        
        // Создаем поглотитель (опционально)
        if (useAbsorber) {
            G4double absorberPosZ = -phantomSize.z()/2 - absorberThickness/2;
            G4Box* absorberSolid = new G4Box("Absorber", phantomSize.x()/2, phantomSize.y()/2, absorberThickness/2);
            G4LogicalVolume* absorberLogical = new G4LogicalVolume(absorberSolid, absorberMaterial, "Absorber");
            new G4PVPlacement(0, G4ThreeVector(0, 0, absorberPosZ), absorberLogical,
                            "Absorber", worldLogical, false, 0);
        }
        
        // Настраиваем визуализацию
        SetupVisualization(worldLogical, phantomLogical);
        
        return worldPhysical;
    }

    virtual void ConstructSDandField() override {
        // Создаем чувствительный детектор
        sensitiveDetector = new SensitiveDetector("PhantomSD", phantomMass);
        
        // Регистрируем его в менеджере
        G4SDManager::GetSDMpointer()->AddNewDetector(sensitiveDetector);
        
        // Назначаем чувствительный детектор фантому
        SetSensitiveDetector(phantomLogical, sensitiveDetector);
    }
    
    G4ThreeVector GetPhantomSize() const { return phantomSize; }

    G4double GetPhantomMass() const { return phantomMass; }

private:
    void SetupVisualization(G4LogicalVolume* worldLV, G4LogicalVolume* phantomLV) {
        // Мировой объем - невидимый
        worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());
        
        // Фантом - синий полупрозрачный
        G4VisAttributes* phantomVis = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.3));
        phantomVis->SetForceSolid(true);
        phantomLV->SetVisAttributes(phantomVis);
        
        // Поглотитель - серый (если используется)
        // if (useAbsorber) {
        //     G4LogicalVolume* absorberLV = FindLogicalVolume("Absorber");
        //     if (absorberLV) {
        //         G4VisAttributes* absorberVis = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.8));
        //         absorberVis->SetForceSolid(true);
        //         absorberLV->SetVisAttributes(absorberVis);
        //     }
        // }
    }
    
    // G4LogicalVolume* FindLogicalVolume(const G4String& name) {
    //     // Простая функция поиска логического объема по имени
    //     G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
    //     for (auto lv : *store) {
    //         if (lv->GetName() == name) return lv;
    //     }
    //     return nullptr;
    // }

    G4double worldSize;
    G4ThreeVector phantomSize;
    G4double absorberThickness;
    G4bool useAbsorber;

    G4double phantomMass = 0.0;
    
    G4Material* absorberMaterial;
    G4Material* phantomMaterial;
    G4LogicalVolume* phantomLogical;
    SensitiveDetector* sensitiveDetector;
};

#endif // DETECTOR_CONSTRUCTION_HPP