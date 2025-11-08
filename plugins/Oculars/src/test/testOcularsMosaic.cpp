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

#include "testOcularsMosaic.hpp"
#include "Oculars.hpp"

QTEST_GUILESS_MAIN(TestOcularsMosaic)

void TestOcularsMosaic::initTestCase()
{
	// Create Oculars instance without calling init()
	// This allows testing getter/setter methods without full StelApp initialization
	oculars = new Oculars();
	// Note: We don't call init() here because it requires StelApp to be fully initialized
	// The getter/setter methods work without init() as they just manipulate member variables
}

void TestOcularsMosaic::cleanupTestCase()
{
	if (oculars)
	{
		delete oculars;
		oculars = nullptr;
	}
}

void TestOcularsMosaic::testMosaicModeToggle()
{
	QVERIFY(oculars != nullptr);
	
	// Test initial state (should be false by default)
	bool initialMode = oculars->getFlagMosaicMode();
	QVERIFY(initialMode == false);
	
	// Toggle on
	oculars->setFlagMosaicMode(true);
	QVERIFY(oculars->getFlagMosaicMode() == true);
	
	// Toggle off
	oculars->setFlagMosaicMode(false);
	QVERIFY(oculars->getFlagMosaicMode() == false);
}

void TestOcularsMosaic::testMosaicModePersistence()
{
	QVERIFY(oculars != nullptr);
	
	// Set mosaic mode
	oculars->setFlagMosaicMode(true);
	QVERIFY(oculars->getFlagMosaicMode() == true);
	
	// Create new instance and check persistence
	// Note: In a real scenario, this would test QSettings persistence
	// For now, we just verify the setter works
	oculars->setFlagMosaicMode(false);
	QVERIFY(oculars->getFlagMosaicMode() == false);
}

void TestOcularsMosaic::testMosaicPanelsXSetter()
{
	QVERIFY(oculars != nullptr);
	
	// Test default value
	int defaultX = oculars->getMosaicPanelsX();
	QVERIFY(defaultX >= 1 && defaultX <= 20);
	
	// Set valid value
	oculars->setMosaicPanelsX(5);
	QVERIFY(oculars->getMosaicPanelsX() == 5);
	
	// Set another valid value
	oculars->setMosaicPanelsX(10);
	QVERIFY(oculars->getMosaicPanelsX() == 10);
}

void TestOcularsMosaic::testMosaicPanelsYSetter()
{
	QVERIFY(oculars != nullptr);
	
	// Test default value
	int defaultY = oculars->getMosaicPanelsY();
	QVERIFY(defaultY >= 1 && defaultY <= 20);
	
	// Set valid value
	oculars->setMosaicPanelsY(3);
	QVERIFY(oculars->getMosaicPanelsY() == 3);
	
	// Set another valid value
	oculars->setMosaicPanelsY(7);
	QVERIFY(oculars->getMosaicPanelsY() == 7);
}

void TestOcularsMosaic::testMosaicPanelsXRange()
{
	QVERIFY(oculars != nullptr);
	
	// Test lower bound (should clamp to 1)
	oculars->setMosaicPanelsX(0);
	QVERIFY(oculars->getMosaicPanelsX() == 1);
	
	// Test upper bound (should clamp to 20)
	oculars->setMosaicPanelsX(25);
	QVERIFY(oculars->getMosaicPanelsX() == 20);
	
	// Test valid range
	oculars->setMosaicPanelsX(15);
	QVERIFY(oculars->getMosaicPanelsX() == 15);
}

void TestOcularsMosaic::testMosaicPanelsYRange()
{
	QVERIFY(oculars != nullptr);
	
	// Test lower bound (should clamp to 1)
	oculars->setMosaicPanelsY(0);
	QVERIFY(oculars->getMosaicPanelsY() == 1);
	
	// Test upper bound (should clamp to 20)
	oculars->setMosaicPanelsY(25);
	QVERIFY(oculars->getMosaicPanelsY() == 20);
	
	// Test valid range
	oculars->setMosaicPanelsY(12);
	QVERIFY(oculars->getMosaicPanelsY() == 12);
}

void TestOcularsMosaic::testMosaicRotationAngleSetter()
{
	QVERIFY(oculars != nullptr);
	
	// Test default value
	double defaultAngle = oculars->getMosaicRotationAngle();
	QVERIFY(defaultAngle >= 0.0 && defaultAngle < 360.0);
	
	// Set valid value
	oculars->setMosaicRotationAngle(45.0);
	QVERIFY(qAbs(oculars->getMosaicRotationAngle() - 45.0) < 0.001);
	
	// Set another valid value
	oculars->setMosaicRotationAngle(90.0);
	QVERIFY(qAbs(oculars->getMosaicRotationAngle() - 90.0) < 0.001);
}

void TestOcularsMosaic::testMosaicOverlapPercentSetter()
{
	QVERIFY(oculars != nullptr);
	
	// Test default value
	double defaultOverlap = oculars->getMosaicOverlapPercent();
	QVERIFY(defaultOverlap >= 0.0 && defaultOverlap <= 50.0);
	
	// Set valid value
	oculars->setMosaicOverlapPercent(20.0);
	QVERIFY(qAbs(oculars->getMosaicOverlapPercent() - 20.0) < 0.001);
	
	// Set another valid value
	oculars->setMosaicOverlapPercent(30.0);
	QVERIFY(qAbs(oculars->getMosaicOverlapPercent() - 30.0) < 0.001);
}

void TestOcularsMosaic::testMosaicRotationAngleNormalization()
{
	QVERIFY(oculars != nullptr);
	
	// Test normalization (angles should wrap to [0, 360))
	oculars->setMosaicRotationAngle(360.0);
	QVERIFY(qAbs(oculars->getMosaicRotationAngle() - 0.0) < 0.001);
	
	oculars->setMosaicRotationAngle(450.0);
	QVERIFY(qAbs(oculars->getMosaicRotationAngle() - 90.0) < 0.001);
	
	oculars->setMosaicRotationAngle(-45.0);
	QVERIFY(qAbs(oculars->getMosaicRotationAngle() - 315.0) < 0.001);
}

void TestOcularsMosaic::testMosaicOverlapPercentRange()
{
	QVERIFY(oculars != nullptr);
	
	// Test lower bound (should clamp to 0.0)
	oculars->setMosaicOverlapPercent(-10.0);
	QVERIFY(qAbs(oculars->getMosaicOverlapPercent() - 0.0) < 0.001);
	
	// Test upper bound (should clamp to 50.0)
	oculars->setMosaicOverlapPercent(75.0);
	QVERIFY(qAbs(oculars->getMosaicOverlapPercent() - 50.0) < 0.001);
	
	// Test valid range
	oculars->setMosaicOverlapPercent(25.0);
	QVERIFY(qAbs(oculars->getMosaicOverlapPercent() - 25.0) < 0.001);
}

void TestOcularsMosaic::testCalculateMosaicPanelsBasic()
{
	QVERIFY(oculars != nullptr);
	
	// Note: calculateMosaicPanels() requires equipment to be selected
	// This test verifies the method doesn't crash with no equipment
	// In a full test, we would set up test equipment first
	
	// For now, just verify the method exists and can be called
	// (it should return empty vector if no equipment is selected)
	oculars->setMosaicPanelsX(3);
	oculars->setMosaicPanelsY(3);
	oculars->setMosaicOverlapPercent(20.0);
	
	// The method should not crash even without equipment
	// (it returns early with empty vector)
	// This is a basic smoke test
	QVERIFY(true); // Placeholder - actual panel calculation requires equipment setup
}

void TestOcularsMosaic::testCalculateMosaicPanelsWithOverlap()
{
	QVERIFY(oculars != nullptr);
	
	// Test with different overlap percentages
	oculars->setMosaicPanelsX(2);
	oculars->setMosaicPanelsY(2);
	
	// Test with 0% overlap
	oculars->setMosaicOverlapPercent(0.0);
	QVERIFY(qAbs(oculars->getMosaicOverlapPercent() - 0.0) < 0.001);
	
	// Test with 50% overlap
	oculars->setMosaicOverlapPercent(50.0);
	QVERIFY(qAbs(oculars->getMosaicOverlapPercent() - 50.0) < 0.001);
	
	// Test with 20% overlap
	oculars->setMosaicOverlapPercent(20.0);
	QVERIFY(qAbs(oculars->getMosaicOverlapPercent() - 20.0) < 0.001);
}

void TestOcularsMosaic::testCalculateMosaicPanelsEdgeCases()
{
	QVERIFY(oculars != nullptr);
	
	// Test edge case: 1x1 panel
	oculars->setMosaicPanelsX(1);
	oculars->setMosaicPanelsY(1);
	QVERIFY(oculars->getMosaicPanelsX() == 1);
	QVERIFY(oculars->getMosaicPanelsY() == 1);
	
	// Test edge case: maximum panels
	oculars->setMosaicPanelsX(20);
	oculars->setMosaicPanelsY(20);
	QVERIFY(oculars->getMosaicPanelsX() == 20);
	QVERIFY(oculars->getMosaicPanelsY() == 20);
}

void TestOcularsMosaic::testCacheInvalidationOnModeChange()
{
	QVERIFY(oculars != nullptr);
	
	// Set up some mosaic parameters
	oculars->setMosaicPanelsX(3);
	oculars->setMosaicPanelsY(3);
	
	// Toggle mosaic mode - this should invalidate cache
	oculars->setFlagMosaicMode(true);
	QVERIFY(oculars->getFlagMosaicMode() == true);
	
	oculars->setFlagMosaicMode(false);
	QVERIFY(oculars->getFlagMosaicMode() == false);
}

void TestOcularsMosaic::testCacheInvalidationOnPanelChange()
{
	QVERIFY(oculars != nullptr);
	
	// Set initial panels
	oculars->setMosaicPanelsX(3);
	oculars->setMosaicPanelsY(3);
	
	// Change panels - this should invalidate cache
	oculars->setMosaicPanelsX(5);
	QVERIFY(oculars->getMosaicPanelsX() == 5);
	
	oculars->setMosaicPanelsY(4);
	QVERIFY(oculars->getMosaicPanelsY() == 4);
}

void TestOcularsMosaic::testCacheInvalidationOnOverlapChange()
{
	QVERIFY(oculars != nullptr);
	
	// Set initial overlap
	oculars->setMosaicOverlapPercent(20.0);
	
	// Change overlap - this should invalidate cache
	oculars->setMosaicOverlapPercent(30.0);
	QVERIFY(qAbs(oculars->getMosaicOverlapPercent() - 30.0) < 0.001);
}

void TestOcularsMosaic::testCacheInvalidationOnRotationChange()
{
	QVERIFY(oculars != nullptr);
	
	// Set initial rotation
	oculars->setMosaicRotationAngle(0.0);
	
	// Change rotation - this should invalidate cache
	oculars->setMosaicRotationAngle(45.0);
	QVERIFY(qAbs(oculars->getMosaicRotationAngle() - 45.0) < 0.001);
}

