/*
 * Copyright (C) 2025 Stellarium Contributors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

#ifndef TESTOCULARSMOSAIC_HPP
#define TESTOCULARSMOSAIC_HPP

#include <QtTest>

class Oculars;

class TestOcularsMosaic : public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();
	
	// Test mosaic mode toggle
	void testMosaicModeToggle();
	void testMosaicModePersistence();
	
	// Test panel property setters
	void testMosaicPanelsXSetter();
	void testMosaicPanelsYSetter();
	void testMosaicPanelsXRange();
	void testMosaicPanelsYRange();
	
	// Test rotation and overlap setters
	void testMosaicRotationAngleSetter();
	void testMosaicOverlapPercentSetter();
	void testMosaicRotationAngleNormalization();
	void testMosaicOverlapPercentRange();
	
	// Test panel calculation (basic)
	void testCalculateMosaicPanelsBasic();
	void testCalculateMosaicPanelsWithOverlap();
	void testCalculateMosaicPanelsEdgeCases();
	
	// Test cache invalidation
	void testCacheInvalidationOnModeChange();
	void testCacheInvalidationOnPanelChange();
	void testCacheInvalidationOnOverlapChange();
	void testCacheInvalidationOnRotationChange();
	
private:
	Oculars* oculars;
};

#endif // TESTOCULARSMOSAIC_HPP

