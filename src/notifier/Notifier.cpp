/*
 *  Manjaro Settings Manager
 *  Roland Singer <roland@manjaro.org>
 *  Ramon Buldó <ramon@manjaro.org>
 *
 *  Copyright (C) 2007 Free Software Foundation, Inc.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "LanguagePackagesCommon.h"
#include "Notifier.h"
#include "NotifierApp.h"
#include "Kernel.h"
#include "KernelModel.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QMap>
#include <QMapIterator>

#include <QDebug>

Notifier::Notifier( QObject* parent ) :
    QTimer( parent )
{
    m_tray.setTitle( QString( "Manjaro Settings Manager" ) );
    m_tray.setIconByName( "manjaro-settings-manager" );
    m_tray.setStatus( KStatusNotifierItem::Passive );

    auto menu = m_tray.contextMenu();

    QAction* msmKernel = new QAction( QIcon( ":/images/resources/tux-manjaro.png" ),
                                      QString( "Kernels" ),
                                      menu );
    QAction* msmLanguagePackages = new QAction(
        QIcon( ":/images/resources/language.png" ),
        QString( "Language packages" ),
        menu );
    menu->addAction( msmKernel );
    menu->addAction( msmLanguagePackages );

    connect( msmKernel, &QAction::triggered, this, [msmKernel, this]()
    {
        QProcess::startDetached( "msm", QStringList() << "-m" << "msm_kernel" );
        m_tray.setStatus( KStatusNotifierItem::Passive );
    } );
    connect( msmLanguagePackages, &QAction::triggered, this, [msmLanguagePackages, this]()
    {
        QProcess::startDetached( "msm", QStringList() << "-m" << "msm_language_packages" );
        m_tray.setStatus( KStatusNotifierItem::Passive );
    } );

    // Set Interval to 24 hours
    setInterval( 24 * 60 * 60 * 1000 );

    connect( this, &Notifier::timeout,
             this, &Notifier::run );
}


void
Notifier::start()
{
    QTimer::singleShot( 30000, this, &Notifier::run );
    QTimer::start();
}


void
Notifier::run()
{
    loadConfiguration();
    if ( checkLanguagePackage && isSystemUpToDate() &&
            !isPacmanUpdating() && hasPacmanEverSynced() )
    {
        cLanguagePackage();
        cKernel();
    }
}


void
Notifier::cLanguagePackage()
{
    // Check if language packages are available
    QList<LanguagePackagesCommon::LanguagePackage> availablePackages, installedPackages, packages;
    QList<LanguagePackagesItem> lpiList { LanguagePackagesCommon::getLanguagePackages() };

    LanguagePackagesCommon::getLanguagePackages( &availablePackages, &installedPackages, lpiList );

    // Check if packages should be ignored
    for ( int i = 0; i < availablePackages.size(); i++ )
    {
        const LanguagePackagesCommon::LanguagePackage* l = &availablePackages.at( i );
        if ( !isPackageIgnored( l->languagePackage, "language_package" ) )
            packages.append( *l );
    }

    if ( !packages.isEmpty() )
    {
        qDebug() << "Missing language packages found.";
        m_tray.setStatus( KStatusNotifierItem::Active );
        m_tray.showMessage( tr( "Manjaro Settings Manager" ),
                            QString( tr( "%n new additional language package(s) available", "", packages.size() ) ),
                            QString( "dialog-information" ),
                            10000 );

        // Add to Config
        for ( int i = 0; i < packages.size(); i++ )
        {
            const LanguagePackagesCommon::LanguagePackage* l = &packages.at( i );
            addToConfig( l->languagePackage, "language_package" );
        }
    }
}


void
Notifier::cKernel()
{
    Notifier::KernelFlags kernelFlags;
    KernelModel kernelModel;
    kernelModel.update();

    QList< Kernel > unsupportedKernels = kernelModel.unsupportedKernels();
    if ( checkUnsupportedKernel && !unsupportedKernels.isEmpty() )
    {
        for ( Kernel kernel : unsupportedKernels )
        {
            if ( isPackageIgnored( kernel.package(), "unsupported_kernel" ) )
            {
                qDebug() << "Ignored unsupported kernel: " << kernel.version();
                continue;
            }
            qDebug() << "Unsupported kernel: " << kernel.version();
            kernelFlags |= KernelFlag::Unsupported;
            if ( checkUnsupportedKernelRunning && kernel.isRunning() )
            {
                kernelFlags |= KernelFlag::Running;
                qDebug() << "Unsupported kernel running: " << kernel.version();
            }
        }
    }

    qDebug() << "Latest installed kernel: " << kernelModel.latestInstalledKernel().version();
    QList<Kernel> newKernels = kernelModel.newerKernels( kernelModel.latestInstalledKernel() );
    QList<Kernel> newLtsRecommendedKernels;
    QList<Kernel> newLtsKernels;
    QList<Kernel> newRecommendedKernels;
    QList<Kernel> newNotIgnoredKernels;
    if ( checkNewKernel )
    {
        for ( Kernel kernel : newKernels )
        {
            if ( isPackageIgnored( kernel.package(), "new_kernel" ) )
            {
                qDebug() << "Ignored new kernel: " << kernel.version();
                continue;
            }
            newNotIgnoredKernels << kernel;
            qDebug() << "Newer kernel: " << kernel.version();
            if ( kernel.isRecommended() && kernel.isLts() )
            {
                qDebug() << "Newer kernel LTS & Recommended: " << kernel.version();
                newLtsRecommendedKernels << kernel;
                newLtsKernels << kernel;
                newRecommendedKernels << kernel;
            }
            else if ( kernel.isLts() )
            {
                qDebug() << "Newer kernel LTS: " << kernel.version();
                newLtsKernels << kernel;
            }
            else if ( kernel.isRecommended() )
            {
                qDebug() << "Newer kernel Recommended: " << kernel.version();
                newRecommendedKernels << kernel;
            }
        }


        if ( checkNewKernelLts && checkNewKernelRecommended )
        {
            if ( !newLtsRecommendedKernels.isEmpty() )
                kernelFlags |= KernelFlag::New;
        }
        else if ( checkNewKernelLts )
        {
            if ( !newLtsKernels.isEmpty() )
                kernelFlags |= KernelFlag::New;
        }
        else if ( checkNewKernelRecommended )
        {
            if ( !newRecommendedKernels.isEmpty() )
                kernelFlags |= KernelFlag::New;
        }
        else
        {
            if ( !newNotIgnoredKernels.isEmpty() )
                kernelFlags |= KernelFlag::New;
        }
    }

    /*
    kernelFlags = 0;
    kernelFlags |= KernelFlag::Unsupported;
    kernelFlags |= KernelFlag::Running;
    kernelFlags |= KernelFlag::New;
    */

    if  ( kernelFlags.testFlag( KernelFlag::Unsupported ) || kernelFlags.testFlag( KernelFlag::New ) )
        m_tray.setStatus( KStatusNotifierItem::Active );

    // Notify about unsupported kernels
    if ( kernelFlags.testFlag( KernelFlag::Unsupported ) )
    {
        QString messageTitle = QString( tr( "Manjaro Settings Manager" ) );
        if ( kernelFlags.testFlag( KernelFlag::Running ) )
        {
            m_tray.showMessage( messageTitle,
                                QString( tr( "Running an unsupported kernel, please update." ) ),
                                QString( "dialog-warning" ),
                                10000 );
        }
        else
        {
            m_tray.showMessage( messageTitle,
                                QString( tr( "Unsupported kernel installed in your system, please remove it." ) ),
                                QString( "dialog-information" ),
                                10000 );
        }
        for ( Kernel kernel : unsupportedKernels )
            addToConfig( kernel.package(), "unsupported_kernel" );
    }

    // Notify about new kernels
    if ( kernelFlags.testFlag( KernelFlag::New ) )
    {
        m_tray.showMessage( QString( tr( "Manjaro Settings Manager" ) ),
                            QString( tr( "Newer kernel is available, please update." ) ),
                            QString( "dialog-information" ),
                            10000 );
        for ( Kernel kernel : newNotIgnoredKernels )
            addToConfig( kernel.package(), "new_kernel" );
    }
}

void
Notifier::loadConfiguration()
{
    QSettings settings( "manjaro", "manjaro-settings-manager" );
    this->checkLanguagePackage = settings.value( "notifications/checkLanguagePackages", true ).toBool();
    this->checkUnsupportedKernel = settings.value( "notifications/checkUnsupportedKernel", true ).toBool();
    this->checkUnsupportedKernelRunning = settings.value( "notifications/checkUnsupportedKernelRunning", false ).toBool();
    this->checkNewKernel = settings.value( "notifications/checkNewKernel", true ).toBool();
    this->checkNewKernelLts = settings.value( "notifications/checkNewKernelLts", false ).toBool();
    this->checkNewKernelRecommended = settings.value( "notifications/checkNewKernelRecommended", true ).toBool();
    this->checkKernel = checkUnsupportedKernel | checkNewKernel;
}


bool
Notifier::isPackageIgnored( const QString package, const QString group )
{
    QSettings settings( "manjaro", "manjaro-settings-manager-Notifier" );
    settings.beginGroup( group );
    int value = settings.value( "notify_count_" + package, "0" ).toInt();
    settings.endGroup();
    return ( value < 2 ) ? false : true;
}


void
Notifier::addToConfig( const QString package, const QString group )
{
    QSettings settings( "manjaro", "manjaro-settings-manager-Notifier" );
    settings.beginGroup( group );
    int value = settings.value( "notify_count_" + package, "0" ).toInt();
    ++value;
    if ( value < 3 )
        settings.setValue( "notify_count_" + package, value );
    settings.endGroup();
}


bool
Notifier::hasPacmanEverSynced()
{
    QString path( "/var/lib/pacman/sync/" );
    QStringList files = QStringList() << "core.db" << "community.db" << "extra.db";
    for ( QString f : files )
    {
        if ( !QFile::exists( path + f ) )
            return false;
    }
    return true;
}


bool
Notifier::isPacmanUpdating()
{
    return QFile::exists( "/var/lib/pacman/db.lck" );
}


bool
Notifier::isSystemUpToDate()
{
    QProcess process;
    process.setEnvironment( QStringList() << "LANG=C" << "LC_MESSAGES=C" );
    process.start( "pacman", QStringList() << "-Sup" );
    if ( !process.waitForFinished() )
    {
        qDebug() << "error: failed to determine if system is up-to-date (pacman)!";
        return false;
    }

    return QString( process.readAll() ).split( "\n", QString::SkipEmptyParts ) ==
           ( QStringList() << ":: Starting full system upgrade..." );
}