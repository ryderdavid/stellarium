/*
 * Mosaic Planner Plugin for Stellarium
 * Copyright (C) 2024
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include "MosaicPlanner.hpp"
#include "StelApp.hpp"
#include "StelCore.hpp"
#include "StelPainter.hpp"
#include "StelProjector.hpp"
#include "StelFileMgr.hpp"
#include "StelModuleMgr.hpp"
#include "StelUtils.hpp"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <cmath>

// Plugin interface implementation
StelModule* MosaicPlannerStelPluginInterface::getStelModule() const
{
	return new MosaicPlanner();
}

StelPluginInfo MosaicPlannerStelPluginInterface::getPluginInfo() const
{
	StelPluginInfo info;
	info.id = "MosaicPlanner";
	info.displayedName = N_("Mosaic Planner");
	info.authors = "Mosaic Planner Developers";
	info.contact = STELLARIUM_DEV_URL;
	info.description = N_("Plan and visualize multi-panel imaging mosaics with configurable overlap and rotation.");
	info.version = "1.0.0";
	info.license = "GNU GPLv2 or later";
	info.startByDefault = false; // Don't auto-start, user must enable
	return info;
}

// Constructor
MosaicPlanner::MosaicPlanner()
	: flagMosaicMode(false)
	, mosaicPanelsX(3)
	, mosaicPanelsY(3)
	, mosaicRotationAngle(0.0)
	, mosaicOverlapPercent(20.0)
	, selectedCCDIndex(-1)
	, selectedTelescopeIndex(-1)
	, selectedLensIndex(-1)
	, mosaicPanelsCacheValid(false)
	, settings(nullptr)
	, lineColor(1.0f, 1.0f, 0.0f) // Yellow by default
{
	setObjectName("MosaicPlanner");
}

MosaicPlanner::~MosaicPlanner()
{
	// Clean up equipment
	qDeleteAll(ccds);
	qDeleteAll(telescopes);
	qDeleteAll(lenses);
}

void MosaicPlanner::init()
{
	StelFileMgr::makeSureDirExistsAndIsWritable(StelFileMgr::getUserDir()+"/modules/MosaicPlanner");
	StelFileMgr::Flags flags = static_cast<StelFileMgr::Flags>(StelFileMgr::Directory|StelFileMgr::Writable);
	QString mosaicIniPath = StelFileMgr::findFile("modules/MosaicPlanner/", flags) + "mosaicplanner.ini";
	
	settings = new QSettings(mosaicIniPath, QSettings::IniFormat);
	
	// Load mosaic settings
	flagMosaicMode = settings->value("mosaic_mode_enabled", false).toBool();
	mosaicPanelsX = qBound(1, settings->value("mosaic_panels_x", 3).toInt(), 20);
	mosaicPanelsY = qBound(1, settings->value("mosaic_panels_y", 3).toInt(), 20);
	mosaicRotationAngle = settings->value("mosaic_rotation_angle", 0.0).toDouble();
	mosaicOverlapPercent = qBound(0.0, settings->value("mosaic_overlap_percent", 20.0).toDouble(), 50.0);
	mosaicPanelsCacheValid = false;
	
	// Load equipment from Oculars ini file (same location)
	loadEquipmentFromOcularsIni();
	
	qDebug() << "MosaicPlanner initialized";
}

void MosaicPlanner::deinit()
{
	if (settings)
	{
		settings->sync();
		delete settings;
		settings = nullptr;
	}
}

void MosaicPlanner::draw(StelCore*)
{
	if (!flagMosaicMode)
		return;
	
	paintMosaicBounds();
}

double MosaicPlanner::getCallOrder(StelModuleActionName actionName) const
{
	if (actionName == StelModule::ActionDraw)
	{
		// Draw after most modules but before final overlays
		StelModule* skyLayerMgr = StelApp::getInstance().getModuleMgr().getModule("StelSkyLayerMgr");
		if (skyLayerMgr)
			return skyLayerMgr->getCallOrder(actionName) + 1.;
		return 10.; // Default order
	}
	return 0;
}

void MosaicPlanner::setFlagMosaicMode(const bool b)
{
	if (b != flagMosaicMode)
	{
		flagMosaicMode = b;
		settings->setValue("mosaic_mode_enabled", b);
		settings->sync();
		mosaicPanelsCacheValid = false;
		emit flagMosaicModeChanged(b);
	}
}

void MosaicPlanner::setMosaicPanelsX(const int panels)
{
	const int clampedPanels = qBound(1, panels, 20);
	if (clampedPanels != mosaicPanelsX)
	{
		mosaicPanelsX = clampedPanels;
		settings->setValue("mosaic_panels_x", mosaicPanelsX);
		settings->sync();
		mosaicPanelsCacheValid = false;
		emit mosaicPanelsXChanged(mosaicPanelsX);
	}
}

void MosaicPlanner::setMosaicPanelsY(const int panels)
{
	const int clampedPanels = qBound(1, panels, 20);
	if (clampedPanels != mosaicPanelsY)
	{
		mosaicPanelsY = clampedPanels;
		settings->setValue("mosaic_panels_y", mosaicPanelsY);
		settings->sync();
		mosaicPanelsCacheValid = false;
		emit mosaicPanelsYChanged(mosaicPanelsY);
	}
}

void MosaicPlanner::setMosaicRotationAngle(const double angle)
{
	const double normalizedAngle = angle - 360.0 * std::floor(angle / 360.0);
	if (std::abs(normalizedAngle - mosaicRotationAngle) > 0.001)
	{
		mosaicRotationAngle = normalizedAngle;
		settings->setValue("mosaic_rotation_angle", mosaicRotationAngle);
		settings->sync();
		mosaicPanelsCacheValid = false;
		emit mosaicRotationAngleChanged(mosaicRotationAngle);
	}
}

void MosaicPlanner::setMosaicOverlapPercent(const double percent)
{
	const double clampedPercent = qBound(0.0, percent, 50.0);
	if (std::abs(clampedPercent - mosaicOverlapPercent) > 0.001)
	{
		mosaicOverlapPercent = clampedPercent;
		settings->setValue("mosaic_overlap_percent", mosaicOverlapPercent);
		settings->sync();
		mosaicPanelsCacheValid = false;
		emit mosaicOverlapPercentChanged(mosaicOverlapPercent);
	}
}

void MosaicPlanner::loadEquipmentFromOcularsIni()
{
	// Load equipment from the same ocular.ini file that Oculars plugin uses
	StelFileMgr::Flags flags = static_cast<StelFileMgr::Flags>(StelFileMgr::Directory|StelFileMgr::Writable);
	QString ocularIniPath = StelFileMgr::findFile("modules/Oculars/", flags) + "ocular.ini";
	
	if (ocularIniPath.isEmpty() || !QFileInfo::exists(ocularIniPath))
	{
		qWarning() << "MosaicPlanner: Cannot find ocular.ini at" << ocularIniPath;
		return;
	}
	
	QSettings ocularSettings(ocularIniPath, QSettings::IniFormat);
	
	// Load CCDs
	int ccdCount = ocularSettings.value("ccd_count", 0).toInt();
	for (int index = 0; index < ccdCount; index++)
	{
		CCD *newCCD = CCD::ccdFromSettings(&ocularSettings, index);
		if (newCCD != Q_NULLPTR)
		{
			ccds.append(newCCD);
		}
	}
	if (!ccds.isEmpty())
		selectedCCDIndex = qMax(0, ocularSettings.value("ccd_index", 0).toInt());
	
	// Load Telescopes
	int telescopeCount = qMax(0, ocularSettings.value("telescope_count", 0).toInt());
	for (int index = 0; index < telescopeCount; index++)
	{
		Telescope *newTelescope = Telescope::telescopeFromSettings(&ocularSettings, index);
		if (newTelescope != Q_NULLPTR)
		{
			telescopes.append(newTelescope);
		}
	}
	if (!telescopes.isEmpty())
		selectedTelescopeIndex = qMax(0, ocularSettings.value("telescope_index", 0).toInt());
	
	// Load Lenses
	int lensCount = ocularSettings.value("lens_count", 0).toInt();
	for (int index = 0; index < lensCount; index++)
	{
		Lens *newLens = Lens::lensFromSettings(&ocularSettings, index);
		if (newLens != Q_NULLPTR)
		{
			lenses.append(newLens);
		}
	}
	selectedLensIndex = ocularSettings.value("lens_index", -1).toInt();
	
	qDebug() << "MosaicPlanner: Loaded" << ccds.count() << "CCDs," 
	         << telescopes.count() << "telescopes," << lenses.count() << "lenses";
}

void MosaicPlanner::calculateMosaicPanels()
{
	cachedMosaicPanels.clear();
	
	// Safety checks
	if (selectedCCDIndex < 0 || selectedCCDIndex >= ccds.count())
		return;
	if (selectedTelescopeIndex < 0 || selectedTelescopeIndex >= telescopes.count())
		return;
	
	CCD *ccd = ccds[selectedCCDIndex];
	if (!ccd) return;
	
	Telescope *telescope = telescopes[selectedTelescopeIndex];
	if (!telescope) return;
	
	Lens *lens = selectedLensIndex >= 0 ? lenses[selectedLensIndex] : Q_NULLPTR;
	
	// Get FOV for the selected equipment
	const double fovX = ccd->getActualFOVx(telescope, lens); // degrees
	const double fovY = ccd->getActualFOVy(telescope, lens); // degrees
	
	// Calculate step size accounting for overlap
	const double overlapFactor = 1.0 - (mosaicOverlapPercent / 100.0);
	const double stepX = fovX * overlapFactor; // degrees
	const double stepY = fovY * overlapFactor; // degrees
	
	// Get mosaic center position (use current view center)
	StelCore *core = StelApp::getInstance().getCore();
	const auto equatProj = core->getProjection(StelCore::FrameEquinoxEqu, StelCore::RefractionMode::RefractionOff);
	const auto altAzProj = core->getProjection(StelCore::FrameAltAz, StelCore::RefractionMode::RefractionOff);
	const auto projector = telescope->isEquatorial() ? equatProj : altAzProj;
	
	Vec2i centerScreen(projector->getViewportPosX() + projector->getViewportWidth() / 2,
			   projector->getViewportPosY() + projector->getViewportHeight() / 2);
	
	Vec3d mosaicCenter;
	projector->unProject(centerScreen[0], centerScreen[1], mosaicCenter);
	
	// Convert mosaic center to spherical coordinates
	double centerRA, centerDec;
	StelUtils::rectToSphe(&centerRA, &centerDec, core->equinoxEquToJ2000(mosaicCenter, StelCore::RefractionOff));
	
	// Rotation in radians
	const double gridRotationRad = mosaicRotationAngle * (M_PI / 180.0);
	const double cosRot = std::cos(gridRotationRad);
	const double sinRot = std::sin(gridRotationRad);
	
	// Calculate panel positions
	cachedMosaicPanels.reserve(mosaicPanelsX * mosaicPanelsY);
	
	for (int i = 0; i < mosaicPanelsX; ++i)
	{
		for (int j = 0; j < mosaicPanelsY; ++j)
		{
			MosaicPanel panel;
			panel.panelIndexX = i;
			panel.panelIndexY = j;
			
			// Calculate local grid position (centered at origin)
			const double localX = (i - (mosaicPanelsX - 1) / 2.0) * stepX;
			const double localY = (j - (mosaicPanelsY - 1) / 2.0) * stepY;
			
			// Apply rotation to grid
			const double rotatedX = localX * cosRot - localY * sinRot;
			const double rotatedY = localX * sinRot + localY * cosRot;
			
			// Convert angular offsets to RA/Dec offsets at the mosaic center
			const double cosDec = std::cos(centerDec);
			const double raOffset = (std::abs(cosDec) > 0.01) ? rotatedX / cosDec : 0.0;
			const double decOffset = rotatedY;
			
			// Calculate panel center RA/Dec
			double panelRA = centerRA + raOffset * (M_PI / 180.0);
			double panelDec = centerDec + decOffset * (M_PI / 180.0);
			
			// Normalize RA to [0, 2π)
			panelRA = StelUtils::fmodpos(panelRA, 2.0 * M_PI);
			
			// Convert to 3D vector
			StelUtils::spheToRect(panelRA, panelDec, panel.center);
			
			// Convert to appropriate coordinate frame
			panel.center = core->j2000ToEquinoxEqu(panel.center, StelCore::RefractionOff);
			
			// Panel rotation includes grid rotation + CCD chip rotation
			panel.rotation = gridRotationRad + ccd->chipRotAngle() * (M_PI / 180.0);
			
			cachedMosaicPanels.append(panel);
		}
	}
	
	mosaicPanelsCacheValid = true;
}

void MosaicPlanner::paintMosaicBounds()
{
	// Safety checks
	if (selectedCCDIndex < 0 || selectedCCDIndex >= ccds.count())
		return;
	if (selectedTelescopeIndex < 0 || selectedTelescopeIndex >= telescopes.count())
		return;
	
	CCD *ccd = ccds[selectedCCDIndex];
	if (!ccd) return;
	
	Telescope *telescope = telescopes[selectedTelescopeIndex];
	if (!telescope) return;
	
	Lens *lens = selectedLensIndex >= 0 ? lenses[selectedLensIndex] : Q_NULLPTR;
	
	StelCore *core = StelApp::getInstance().getCore();
	const auto equatProj = core->getProjection(StelCore::FrameEquinoxEqu, StelCore::RefractionMode::RefractionOff);
	const auto altAzProj = core->getProjection(StelCore::FrameAltAz, StelCore::RefractionMode::RefractionOff);
	const auto projector = telescope->isEquatorial() ? equatProj : altAzProj;
	
	// Always recalculate panels - cache is invalidated when equipment or parameters change
	// Note: We recalculate each frame to ensure mosaic follows view center
	calculateMosaicPanels();
	
	// Render each panel
	for (const auto& panel : cachedMosaicPanels)
	{
		drawMosaicPanelFrame(panel, projector, *ccd, lens);
	}
}

void MosaicPlanner::drawMosaicPanelFrame(const MosaicPanel& panel, const StelProjectorP& projector, const CCD& ccd, const Lens* lens)
{
	Telescope *telescope = telescopes[selectedTelescopeIndex];
	if (!telescope) return;
	
	StelCore *core = StelApp::getInstance().getCore();
	
	// Convert panel center to screen coordinates
	Vec3f panelCenterWin;
	projector->project(panel.center, panelCenterWin);
	
	// Get azimuth and elevation of panel center for rotation matrix
	Vec3d panelCenterAltAz = core->equinoxEquToAltAz(panel.center, StelCore::RefractionAuto);
	double azimuth, elevation;
	StelUtils::rectToSphe(&azimuth, &elevation, panelCenterAltAz);
	
	// Create rotation matrix (includes grid rotation + CCD rotation)
	const auto derotate = Mat4f::rotation(Vec3f(0,0,1), azimuth) *
			      Mat4f::rotation(Vec3f(0,1,0), -elevation) *
			      Mat4f::rotation(Vec3f(1,0,0), panel.rotation);
	
	// Get FOV
	const double fovX = ccd.getActualFOVx(telescope, lens) * (M_PI/180);
	const double fovY = ccd.getActualFOVy(telescope, lens) * (M_PI/180);
	
	const float tanFovX = std::tan(fovX/2);
	const float tanFovY = std::tan(fovY/2);
	
	// Draw frame similar to drawSensorFrameAndOverlay but centered on panel
	StelPainter painter(projector);
	painter.setLineSmooth(true);
	painter.setColor(lineColor);
	
	const int numPointsPerLine = 30;
	std::vector<Vec2f> lineLoopPoints;
	lineLoopPoints.reserve(numPointsPerLine * 4);
	
	// Left line
	for(int n = 0; n < numPointsPerLine; ++n)
	{
		const auto x = 1;
		const auto y = tanFovX;
		const auto z = tanFovY * (2.f / (numPointsPerLine - 1) * n - 1);
		Vec3f skyPoint = derotate * Vec3f(x,y,z);
		Vec3f win;
		projector->project(skyPoint, win);
		lineLoopPoints.emplace_back(Vec2f(win[0], win[1]));
	}
	// Top line
	for(int n = 1; n < numPointsPerLine; ++n)
	{
		const auto x = 1;
		const auto y = -tanFovX * (2.f / (numPointsPerLine - 1) * n - 1);
		const auto z = tanFovY;
		Vec3f skyPoint = derotate * Vec3f(x,y,z);
		Vec3f win;
		projector->project(skyPoint, win);
		lineLoopPoints.emplace_back(Vec2f(win[0], win[1]));
	}
	// Right line
	for(int n = 1; n < numPointsPerLine; ++n)
	{
		const auto x = 1;
		const auto y = -tanFovX;
		const auto z = tanFovY * (1 - 2.f / (numPointsPerLine - 1) * n);
		Vec3f skyPoint = derotate * Vec3f(x,y,z);
		Vec3f win;
		projector->project(skyPoint, win);
		lineLoopPoints.emplace_back(Vec2f(win[0], win[1]));
	}
	// Bottom line
	for(int n = 1; n < numPointsPerLine - 1; ++n)
	{
		const auto x = 1;
		const auto y = tanFovX * (1 - 2.f / (numPointsPerLine - 1) * n);
		const auto z = -tanFovY;
		Vec3f skyPoint = derotate * Vec3f(x,y,z);
		Vec3f win;
		projector->project(skyPoint, win);
		lineLoopPoints.emplace_back(Vec2f(win[0], win[1]));
	}
	
	// Draw the frame
	if (!lineLoopPoints.empty())
	{
		painter.enableClientStates(true);
		painter.setVertexPointer(2, GL_FLOAT, lineLoopPoints.data());
		painter.drawFromArray(StelPainter::LineLoop, static_cast<int>(lineLoopPoints.size()), 0, false);
		painter.enableClientStates(false);
	}
}

