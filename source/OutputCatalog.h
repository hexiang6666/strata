////////////////////////////////////////////////////////////////////////////////
//
// This file is part of Strata.
//
// Strata is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// Strata is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// Strata.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2010-2018 Albert Kottke
//
////////////////////////////////////////////////////////////////////////////////

#ifndef OUTPUT_CATALOG_H
#define OUTPUT_CATALOG_H

#include <QAbstractTableModel>
#include <QDataStream>
#include <QJsonObject>
#include <QStringList>
#include <QVector>

#include "SoilTypeCatalog.h"

class AbstractCalculator;
class AbstractOutput;
class AbstractOutputCatalog;
class Dimension;
class MotionLibrary;
class ProfilesOutputCatalog;
class RatiosOutputCatalog;
class SoilProfile;
class SoilTypesOutputCatalog;
class SpectraOutputCatalog;
class TextLog;
class TimeSeriesOutputCatalog;

class OutputCatalog : public QAbstractTableModel
{
    Q_OBJECT

    friend QDataStream & operator<< (QDataStream & out, const OutputCatalog* oc);
    friend QDataStream & operator>> (QDataStream & in, OutputCatalog* oc);

public:
    explicit OutputCatalog(QObject *parent = nullptr);

    enum Columns {
        EnabledColumn,
        SiteColumn,
        MotionColumn
    };

    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;

    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    virtual Qt::ItemFlags flags(const QModelIndex & index) const;

    ProfilesOutputCatalog* profilesCatalog();
    RatiosOutputCatalog* ratiosCatalog();
    SoilTypesOutputCatalog* soilTypesCatalog();
    SpectraOutputCatalog* spectraCatalog();
    TimeSeriesOutputCatalog* timeSeriesCatalog();

    TextLog* log();

    const QVector<double>& depth() const;
    const QVector<double>& time(int motion) const;

    Dimension* frequency();
    Dimension* period();

    double damping() const;

    int motionCount() const;
    int siteCount() const;

    const QString & title() const;
    const QString & filePrefix() const;

    AbstractOutput* setSelectedOutput(int index);

    bool enabledAt(int site, int motion) const;
    bool enabledAt(int row) const;

    bool siteEnabled(int row) const;
    void setSiteEnabled(int row, bool enabled);

    bool motionEnabled(int row) const;
    void setMotionEnabled(int row, bool enabled);

    int siteNumberAt(int row) const;
    const QString motionNameAt(int row) const;

    bool timesAreNeeded() const;
    bool periodIsNeeded() const;
    bool frequencyIsNeeded() const;

    QStringList outputNames() const;

    const QList<AbstractOutput*> & outputs() const;

    /*! Prepare the OutputCatalog to save data
     */
    void initialize(int siteCount, MotionLibrary* motionLibrary);

    /*! Finalize the data by computing the statistics
     */
    void finalize();

    /*! Save the results from a calculation
     */
    void saveResults(int motion, AbstractCalculator* const calculator);

    /*! Remove the last site from the output
     */
    void removeLastSite();

    /*! Export the data to files
     *
     * \param path location to save the files
     * \param separator separate the columns of data with this symbol
     * \param prefix prefix to append to the start of filenames
     */
    void exportData(const QString & path, const QString & separator = ",", const QString & prefix = "" );

    //! Compute the statistics of all of the outputs
    void computeStats();

    void fromJson(const QJsonObject &json);
    QJsonObject toJson() const;

signals:
    void timesAreNeededChanged(bool timesAreNeeded);
    void periodIsNeededChanged(bool periodIsNeeded);
    void frequencyIsNeededChanged(bool frequencyIsNeeded);

    void enabledChanged(int row);
    void wasModified();

public slots:
    void setTitle(const QString &  title);
    void setFilePrefix(const QString & prefix);
    void setDamping(double damping);

    //! Clear all saved data
    void clear();

    //! Set the catalogs are read-only
    void setReadOnly(bool readOnly);

protected slots:
    void setTimesAreNeeded(bool timesAreNeeded);
    void setPeriodIsNeeded(bool periodIsNeeded);
    void setFrequencyIsNeeded(bool frequencyIsNeeded);

protected:
    /*! Create a vector of appropriately spaced depths.
     * The vector uses the following scheme of layer thicknesses:
     *  English units:
     *   0 to  20 ft: 1 ft layers
     *  20 to  60 ft: 2 ft layers
     *  60 to 160 ft: 5 ft layers
     * 160 to 360 ft: 10 ft layers
     * beyond 360 ft: 20 ft layers
     *
     * If the vector has already been created, then it is extended to reach
     * the desired depth.
     *
     * \param maxDepth maximum depth of the vector units of Units->system()
     */
    void populateDepthVector(double maxDepth);

    //! List of all enabled outputs
    QList<AbstractOutput*> _outputs;

    //! Title of the project
    QString _title;

    //! Filename prefix for the exported text files
    QString _filePrefix;

    //! If the output is enabled
    QList<QList<bool> > _enabled;

    //! Depth values
    QVector<double> _depth;

    //! Names of input motions
    QStringList _motionNames;

    //! Number of sites analyzed
    int _siteCount;

    //! Number of motions analyzed
    int _motionCount;

    //! Time vectors for the various time series
    QList<QVector<double> > _time;

    //! If the times are needed
    bool _timesAreNeeded;

    //! Frequency
    Dimension* _frequency;

    //! If the frequency is needed
    bool _frequencyIsNeeded;

    //! Period
    Dimension* _period;

    //! If the period is needed
    bool _periodIsNeeded;

    //! Damping of the single-degree-of-freedom system
    double _damping;

    //! Catalogs of output
    ProfilesOutputCatalog* _profilesOutputCatalog;
    RatiosOutputCatalog* _ratiosOutputCatalog;
    SoilTypesOutputCatalog* _soilTypesOutputCatalog;
    SpectraOutputCatalog* _spectraOutputCatalog;
    TimeSeriesOutputCatalog* _timeSeriesOutputCatalog;

    //! List of the catalogs
    QList<AbstractOutputCatalog*> _catalogs;

    //! Log of the analysis
    TextLog* _log;

    //! The output the is currently selected by the view
    AbstractOutput* _selectedOutput;
};

#endif // OUTPUT_CATALOG_H
