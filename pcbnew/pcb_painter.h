/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2013 CERN
 * Copyright (C) 2016 KiCad Developers, see AUTHORS.txt for contributors.
 * @author Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
 * @author Maciej Suminski <maciej.suminski@cern.ch>
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
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef __CLASS_PCB_PAINTER_H
#define __CLASS_PCB_PAINTER_H

#include <painter.h>

#include <memory>


class EDA_ITEM;
class COLORS_DESIGN_SETTINGS;
class PCB_DISPLAY_OPTIONS;

class BOARD_ITEM;
class BOARD;
class VIA;
class TRACK;
class D_PAD;
class DRAWSEGMENT;
class MODULE;
class ZONE_CONTAINER;
class TEXTE_PCB;
class TEXTE_MODULE;
class DIMENSION;
class PCB_TARGET;
class MARKER_PCB;

namespace KIGFX
{
class GAL;

/**
 * Class PCB_RENDER_SETTINGS
 * Stores PCB specific render settings.
 */
class PCB_RENDER_SETTINGS : public RENDER_SETTINGS
{
public:
    friend class PCB_PAINTER;

    ///> Flags to control clearance lines visibility
    enum CLEARANCE_MODE
    {
        CL_NONE             = 0x00,

        // Object type
        CL_PADS             = 0x01,
        CL_VIAS             = 0x02,
        CL_TRACKS           = 0x04,

        // Existence
        CL_NEW              = 0x08,
        CL_EDITED           = 0x10,
        CL_EXISTING         = 0x20
    };

    ///> Determines how zones should be displayed
    enum DISPLAY_ZONE_MODE
    {
        DZ_HIDE_FILLED = 0,
        DZ_SHOW_FILLED,
        DZ_SHOW_OUTLINED
    };

    PCB_RENDER_SETTINGS();

    /// @copydoc RENDER_SETTINGS::ImportLegacyColors()
    void ImportLegacyColors( const COLORS_DESIGN_SETTINGS* aSettings ) override;

    /**
     * Function LoadDisplayOptions
     * Loads settings related to display options (high-contrast mode, full or outline modes
     * for vias/pads/tracks and so on).
     * @param aOptions are settings that you want to use for displaying items.
     */
    void LoadDisplayOptions( const PCB_DISPLAY_OPTIONS& aOptions, bool aShowPageLimits );

    /// @copydoc RENDER_SETTINGS::GetColor()
    virtual const COLOR4D& GetColor( const VIEW_ITEM* aItem, int aLayer ) const override;

    /**
     * Function SetSketchMode
     * Turns on/off sketch mode for given item layer.
     * @param aItemLayer is the item layer that is changed.
     * @param aEnabled decides if it is drawn in sketch mode (true for sketched mode,
     * false for filled mode).
     */
    inline void SetSketchMode( int aItemLayer, bool aEnabled )
    {
        m_sketchMode[aItemLayer] = aEnabled;
    }

    /**
     * Function GetSketchMode
     * Returns sketch mode setting for a given item layer.
     * @param aItemLayer is the item layer that is changed.
     */
    inline bool GetSketchMode( int aItemLayer ) const
    {
        return m_sketchMode[aItemLayer];
    }

    /**
     * Turns on/off sketch mode for graphic items (DRAWSEGMENTs, texts).
     * @param aEnabled decides if it is drawn in sketch mode (true for sketched mode,
     * false for filled mode).
     */
    inline void SetSketchModeGraphicItems( bool aEnabled )
    {
        m_sketchBoardGfx = aEnabled;
    }

    /**
     * Turns on/off drawing outline and hatched lines for zones.
     */
    void EnableZoneOutlines( bool aEnabled )
    {
        m_zoneOutlines = aEnabled;
    }

    inline bool IsBackgroundDark() const override
    {
        auto luma = m_layerColors[ LAYER_PCB_BACKGROUND ].GetBrightness();

        return luma < 0.5;
    }

    const COLOR4D& GetBackgroundColor() override { return m_layerColors[ LAYER_PCB_BACKGROUND ]; }

    void SetBackgroundColor( const COLOR4D& aColor ) override
    {
        m_layerColors[ LAYER_PCB_BACKGROUND ] = aColor;
    }

    const COLOR4D& GetGridColor() override { return m_layerColors[ LAYER_GRID ]; }

    const COLOR4D& GetCursorColor() override { return m_layerColors[ LAYER_CURSOR ]; }

    inline bool GetCurvedRatsnestLinesEnabled() const
    {
        return m_curvedRatsnestlines;
    }

    inline bool GetGlobalRatsnestLinesEnabled() const
    {
        return m_globalRatsnestlines;
    }

protected:
    ///> Flag determining if items on a given layer should be drawn as an outline or a filled item
    bool    m_sketchMode[GAL_LAYER_ID_END];

    ///> Flag determining if board graphic items should be outlined or stroked
    bool    m_sketchBoardGfx;

    ///> Flag determining if footprint graphic items should be outlined or stroked
    bool    m_sketchFpGfx;

    ///> Flag determining if footprint text items should be outlined or stroked
    bool    m_sketchFpTxtfx;

    ///> Flag determining if pad numbers should be visible
    bool    m_padNumbers;

    ///> Flag determining if net names should be visible for pads
    bool    m_netNamesOnPads;

    ///> Flag determining if net names should be visible for tracks
    bool    m_netNamesOnTracks;

    ///> Flag determining if net names should be visible for vias
    bool    m_netNamesOnVias;

    ///> Flag determining if zones should have outlines drawn
    bool    m_zoneOutlines;

    ///> Flag determining if ratsnest lines should be drawn curved
    bool    m_curvedRatsnestlines = true;

    ///> Flag determining if ratsnest lines are shown by default
    bool    m_globalRatsnestlines = true;

    ///> Maximum font size for netnames (and other dynamically shown strings)
    static const double MAX_FONT_SIZE;

    ///> Option for different display modes for zones
    DISPLAY_ZONE_MODE m_displayZone;

    ///> Clearance visibility settings
    int m_clearance;

    ///> Color used for highlighting selection candidates
    COLOR4D m_selectionCandidateColor;
};


/**
 * Class PCB_PAINTER
 * Contains methods for drawing PCB-specific items.
 */
class PCB_PAINTER : public PAINTER
{
public:
    PCB_PAINTER( GAL* aGal );

    /// @copydoc PAINTER::ApplySettings()
    virtual void ApplySettings( const RENDER_SETTINGS* aSettings ) override
    {
        m_pcbSettings = *static_cast<const PCB_RENDER_SETTINGS*>( aSettings );
    }

    /// @copydoc PAINTER::GetSettings()
    virtual PCB_RENDER_SETTINGS* GetSettings() override
    {
        return &m_pcbSettings;
    }

    /// @copydoc PAINTER::Draw()
    virtual bool Draw( const VIEW_ITEM* aItem, int aLayer ) override;

protected:
    PCB_RENDER_SETTINGS m_pcbSettings;

    // Drawing functions for various types of PCB-specific items
    void draw( const TRACK* aTrack, int aLayer );
    void draw( const VIA* aVia, int aLayer );
    void draw( const D_PAD* aPad, int aLayer );
    void draw( const DRAWSEGMENT* aSegment, int aLayer );
    void draw( const TEXTE_PCB* aText, int aLayer );
    void draw( const TEXTE_MODULE* aText, int aLayer );
    void draw( const MODULE* aModule, int aLayer );
    void draw( const ZONE_CONTAINER* aZone, int aLayer );
    void draw( const DIMENSION* aDimension, int aLayer );
    void draw( const PCB_TARGET* aTarget );
    void draw( const MARKER_PCB* aMarker );

    /**
     * Function getLineThickness()
     * Get the thickness to draw for a line (e.g. 0 thickness lines
     * get a minimum value).
     * @param aActualThickness line own thickness
     * @return the thickness to draw
     */
    int getLineThickness( int aActualThickness ) const;

    /**
     * Return drill shape of a pad.
     */
    virtual int getDrillShape( const D_PAD* aPad ) const;

    /**
     * Return drill size for a pad (internal units).
     */
    virtual VECTOR2D getDrillSize( const D_PAD* aPad ) const;

    /**
     * Return drill diameter for a via (internal units).
     */
    virtual int getDrillSize( const VIA* aVia ) const;
};
} // namespace KIGFX

#endif /* __CLASS_PAINTER_H */
