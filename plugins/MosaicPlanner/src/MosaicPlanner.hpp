/*
 * Mosaic Planner Plugin for Stellarium
 * Copyright (C) 2024
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef MOSAICPLANNER_HPP
#define MOSAICPLANNER_HPP

#include "StelModule.hpp"
#include "StelProjectorType.hpp"
#include "VecMath.hpp"
#include "CCD.hpp"
#include "Telescope.hpp"
#include "Lens.hpp"

#include <QSettings>
#include <QVector>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

class StelPainter;
class StelProjector;

//! Main class of the Mosaic Planner plugin
class MosaicPlanner : public StelModule
{
	Q_OBJECT
	
	Q_PROPERTY(bool flagMosaicMode READ getFlagMosaicMode WRITE setFlagMosaicMode NOTIFY flagMosaicModeChanged)
	Q_PROPERTY(int mosaicPanelsX READ getMosaicPanelsX WRITE setMosaicPanelsX NOTIFY mosaicPanelsXChanged)
	Q_PROPERTY(int mosaicPanelsY READ getMosaicPanelsY WRITE setMosaicPanelsY NOTIFY mosaicPanelsYChanged)
	Q_PROPERTY(double mosaicRotationAngle READ getMosaicRotationAngle WRITE setMosaicRotationAngle NOTIFY mosaicRotationAngleChanged)
	Q_PROPERTY(double mosaicOverlapPercent READ getMosaicOverlapPercent WRITE setMosaicOverlapPercent NOTIFY mosaicOverlapPercentChanged)
	
public:
	MosaicPlanner();
	~MosaicPlanner() override;
	
	// Methods defined in the StelModule class
	void init() override;
	void deinit() override;
	void draw(StelCore* core) override;
	double getCallOrder(StelModuleActionName actionName) const override;
	
	// Property getters
	bool getFlagMosaicMode() const { return flagMosaicMode; }
	int getMosaicPanelsX() const { return mosaicPanelsX; }
	int getMosaicPanelsY() const { return mosaicPanelsY; }
	double getMosaicRotationAngle() const { return mosaicRotationAngle; }
	double getMosaicOverlapPercent() const { return mosaicOverlapPercent; }
	
public slots:
	void setFlagMosaicMode(const bool b);
	void setMosaicPanelsX(const int panels);
	void setMosaicPanelsY(const int panels);
	void setMosaicRotationAngle(const double angle);
	void setMosaicOverlapPercent(const double percent);
	
signals:
	void flagMosaicModeChanged(bool b);
	void mosaicPanelsXChanged(int panels);
	void mosaicPanelsYChanged(int panels);
	void mosaicRotationAngleChanged(double angle);
	void mosaicOverlapPercentChanged(double percent);
	
private:
	struct MosaicPanel {
		Vec3d center;        // 3D sky position of panel center
		double rotation;     // Panel rotation in radians
		int panelIndexX;     // Grid X position (0 to panelsX-1)
		int panelIndexY;     // Grid Y position (0 to panelsY-1)
	};
	
	void paintMosaicBounds();
	void calculateMosaicPanels();
	void drawMosaicPanelFrame(const MosaicPanel& panel, const StelProjectorP& projector, const CCD& ccd, const Lens* lens);
	void loadEquipmentFromOcularsIni();
	
	// Mosaic configuration
	bool flagMosaicMode;
	int mosaicPanelsX;
	int mosaicPanelsY;
	double mosaicRotationAngle;  // degrees
	double mosaicOverlapPercent; // 0-50%
	
	// Equipment lists
	QList<CCD*> ccds;
	QList<Telescope*> telescopes;
	QList<Lens*> lenses;
	
	// Selected equipment
	int selectedCCDIndex;
	int selectedTelescopeIndex;
	int selectedLensIndex;
	
	// Calculated panels
	QVector<MosaicPanel> cachedMosaicPanels;
	bool mosaicPanelsCacheValid;
	
	// Settings
	QSettings* settings;
	
	// Visual properties
	Vec3f lineColor; // Color for mosaic panel outlines
};

#include <QObject>
#include "StelPluginInterface.hpp"

//! This class is used by Qt to manage a plug-in interface
class MosaicPlannerStelPluginInterface : public QObject, public StelPluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID StelPluginInterface_iid)
	Q_INTERFACES(StelPluginInterface)
public:
	StelModule* getStelModule() const override;
	StelPluginInfo getPluginInfo() const override;
	QObjectList getExtensionList() const override { return QObjectList(); }
};

#endif /* MOSAICPLANNER_HPP */

