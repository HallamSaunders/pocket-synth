/*
  ==============================================================================

    Spacing.cpp
    Created: 8 Mar 2025 12:18:19am
    Author:  Hallam Saunders

  ==============================================================================
*/

#include "Spacing.h"

#include <JuceHeader.h>

juce::Grid Spacing::getGridLayout()
{
    juce::Grid grid;

	// Create a 16x16 grid with padding
    juce::Array<juce::Grid::TrackInfo> tracks;
    for (int i = 0; i < 16; ++i)
        tracks.add(juce::Grid::Fr(1));

    grid.templateColumns = tracks;
    grid.templateRows = tracks;


    grid.setGap(juce::Grid::Px(margin));

    return grid;
}

juce::Grid Spacing::getMainGridLayout()
{
	juce::Grid grid;

	// Create an 6x1 grid with padding
	juce::Array<juce::Grid::TrackInfo> row_tracks;
	row_tracks.add(juce::Grid::Fr(1));
	row_tracks.add(juce::Grid::Fr(1));
	row_tracks.add(juce::Grid::Fr(10));
	row_tracks.add(juce::Grid::Fr(10));
	row_tracks.add(juce::Grid::Fr(3));
	row_tracks.add(juce::Grid::Fr(3));
	grid.templateRows = row_tracks;

	juce::Array<juce::Grid::TrackInfo> column_tracks;
	column_tracks.add(juce::Grid::Fr(1));
	grid.templateColumns = column_tracks;

	grid.setGap(juce::Grid::Px(margin));

	return grid;
}

juce::Grid Spacing::getTitleBarGridLayout()
{
	juce::Grid grid;

	// Create a 1x5 grid with padding
	juce::Array<juce::Grid::TrackInfo> row_tracks;
	row_tracks.add(juce::Grid::Fr(1));
	grid.templateRows = row_tracks;

	juce::Array<juce::Grid::TrackInfo> column_tracks;
	column_tracks.add(juce::Grid::Fr(1));
	column_tracks.add(juce::Grid::Fr(1));
	column_tracks.add(juce::Grid::Fr(4));
	column_tracks.add(juce::Grid::Fr(1));
	column_tracks.add(juce::Grid::Fr(1));
	grid.templateColumns = column_tracks;

	grid.setGap(juce::Grid::Px(margin));

	return grid;
}

juce::Grid Spacing::getPresetBarGridLayout()
{
	juce::Grid grid;

	// Create a 1x5 grid with padding
	juce::Array<juce::Grid::TrackInfo> row_tracks;
	row_tracks.add(juce::Grid::Fr(1));
	grid.templateRows = row_tracks;

	juce::Array<juce::Grid::TrackInfo> column_tracks;
	column_tracks.add(juce::Grid::Fr(1));
	column_tracks.add(juce::Grid::Fr(1));
	column_tracks.add(juce::Grid::Fr(4));
	column_tracks.add(juce::Grid::Fr(1));
	column_tracks.add(juce::Grid::Fr(1));
	grid.templateColumns = column_tracks;

	grid.setGap(juce::Grid::Px(margin));

	return grid;
}

juce::Grid Spacing::getOscillatorGridLayout()
{
	juce::Grid grid;

	// Create a 10x24 grid with padding
	juce::Array<juce::Grid::TrackInfo> row_tracks;
	for (int i = 0; i < 10; ++i)
        row_tracks.add(juce::Grid::Fr(1));
	grid.templateRows = row_tracks;

	juce::Array<juce::Grid::TrackInfo> column_tracks;
	for (int i = 0; i < 24; ++i)
		column_tracks.add(juce::Grid::Fr(1));
	grid.templateColumns = column_tracks;

	grid.setGap(juce::Grid::Px(margin));

	return grid;
}

juce::Grid Spacing::getGlobalControlsGridLayout()
{
	juce::Grid grid;

	// Create a 3x24 grid with padding
	juce::Array<juce::Grid::TrackInfo> row_tracks;
	row_tracks.add(juce::Grid::Fr(1));
	row_tracks.add(juce::Grid::Fr(1));
	row_tracks.add(juce::Grid::Fr(1));
	grid.templateRows = row_tracks;

	juce::Array<juce::Grid::TrackInfo> column_tracks;
	for (int i = 0; i < 24; ++i)
		column_tracks.add(juce::Grid::Fr(1));
	grid.templateColumns = column_tracks;

	grid.setGap(juce::Grid::Px(margin));

	return grid;
}