/*
  Copyright 2014 Statoil ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef ECLIPSE_GRIDPROPERTY_HPP_
#define ECLIPSE_GRIDPROPERTY_HPP_

#include <string>
#include <vector>

#include <ert/ecl/EclKW.hpp>

#include <opm/parser/eclipse/EclipseState/Grid/GridPropertyInitializers.hpp>

/*
  This class implemenents a class representing properties which are
  define over an ECLIPSE grid, i.e. with one value for each logical
  cartesian cell in the grid.
*/

namespace Opm {

    class Box;
    class DeckItem;
    class DeckKeyword;
    class EclipseGrid;

template< typename T >
class GridPropertySupportedKeywordInfo {

    public:
        GridPropertySupportedKeywordInfo() = default;

        GridPropertySupportedKeywordInfo(
            const std::string& name,
            GridPropertyInitFunction< T > initializer,
            GridPropertyPostFunction< T > postProcessor,
            const std::string& dimString );

        GridPropertySupportedKeywordInfo(
                const std::string& name,
                GridPropertyInitFunction< T > initializer,
                const std::string& dimString);

        /* this is a convenience constructor which can be used if the default
         * value for the grid property is just a constant.
         */
        GridPropertySupportedKeywordInfo(
                const std::string& name,
                const T defaultValue,
                const std::string& dimString );

        GridPropertySupportedKeywordInfo(
                const std::string& name,
                const T defaultValue,
                GridPropertyPostFunction< T > postProcessor,
                const std::string& dimString );

        const std::string& getKeywordName() const;
        const std::string& getDimensionString() const;
        const GridPropertyInitFunction< T >& initializer() const;
        const GridPropertyPostFunction< T >& postProcessor() const;

    private:
        std::string m_keywordName;
        GridPropertyInitFunction< T > m_initializer;
        GridPropertyPostFunction< T > m_postProcessor;
        std::string m_dimensionString;
};

template< typename T >
class GridProperty {
public:
    typedef GridPropertySupportedKeywordInfo<T> SupportedKeywordInfo;

    GridProperty( size_t nx, size_t ny, size_t nz, const SupportedKeywordInfo& kwInfo );

    size_t getCartesianSize() const;
    size_t getNX() const;
    size_t getNY() const;
    size_t getNZ() const;


    T iget(size_t index) const;
    T iget(size_t i , size_t j , size_t k) const;
    void iset(size_t index, T value);
    void iset(size_t i , size_t j , size_t k , T value);

    const std::vector<T>& getData() const;

    bool containsNaN() const;
    const std::string& getDimensionString() const;

    void multiplyWith( const GridProperty<T>& );
    void multiplyValueAtIndex( size_t index, T factor );
    void maskedSet( T value, const std::vector< bool >& mask );
    void maskedMultiply( T value, const std::vector< bool >& mask );
    void maskedAdd( T value, const std::vector< bool >& mask );
    void maskedCopy( const GridProperty< T >& other, const std::vector< bool >& mask );
    void initMask( T value, std::vector<bool>& mask ) const;

    /**
       Due to the convention where it is only neceassary to supply the
       top layer of the petrophysical properties we can unfortunately
       not enforce that the number of elements elements in the
       deckkeyword equals nx*ny*nz.
    */

    void loadFromDeckKeyword( const DeckKeyword& );
    void loadFromDeckKeyword( std::shared_ptr<const Box>, const DeckKeyword& );

    void copyFrom( const GridProperty< T >&, std::shared_ptr< const Box > );
    void scale( T scaleFactor, std::shared_ptr< const Box > );
    void add( T shiftValue, std::shared_ptr< const Box > );
    void setScalar( T value, std::shared_ptr< const Box > );

    const std::string& getKeywordName() const;
    const SupportedKeywordInfo& getKeywordInfo() const;

    void runPostProcessor();

    ERT::EclKW<T> getEclKW() const;
    ERT::EclKW<T> getEclKW( std::shared_ptr< const EclipseGrid > ) const;

    /**
       Will check that all elements in the property are in the closed
       interval [min,max].
    */
    void checkLimits( T min, T max ) const;


private:
    const DeckItem& getDeckItem( const DeckKeyword& );
    void setDataPoint(size_t sourceIdx, size_t targetIdx, const DeckItem& deckItem);

    size_t      m_nx,m_ny,m_nz;
    SupportedKeywordInfo m_kwInfo;
    std::vector<T> m_data;
    bool m_hasRunPostProcessor = false;
};

}
#endif
