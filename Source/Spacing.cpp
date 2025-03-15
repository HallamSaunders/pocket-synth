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