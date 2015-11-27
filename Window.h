#ifndef WINDOW_H
#define WINDOW_H

#include "GLViewer.h"
#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QCheckBox>
#include <QGroupBox>
#include <QComboBox>
#include <QSlider>
#include <QLCDNumber>
#include <QSpinBox>
#include <QImage>
#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>
#include <QDockWidget>
#include <QGroupBox>
#include <QButtonGroup>
#include <QMenuBar>
#include <QApplication>
#include <QLayout>
#include <QLabel>
#include <QProgressBar>
#include <QColorDialog>
#include <QLCDNumber>
#include <QPixmap>
#include <QFrame>
#include <QSplitter>
#include <QMenu>
#include <QScrollArea>
#include <QCoreApplication>
#include <QFont>
#include <QSizePolicy>
#include <QImageReader>
#include <QStatusBar>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <QFormLayout>
#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "QTUtils.h"


/*!
 *  \brief A class that creates the User Interface. 
 *
 *  Contains:
 *  - Interface items 
 *  - SLOTs functions that are used to handle user actions 
 */
class Window : public QMainWindow {
    Q_OBJECT
public:
    Window();
    virtual ~Window();

    static void showStatusMessage (const QString & msg);  
    
public slots :
    void renderRayImage ();
    void setBGColor ();
    void showRayImage ();
    void exportGLImage ();
    void exportRayImage ();
    void about ();

    /*Windows only*/
    void interactiveWarning (bool b);
    
    /* Program parameters */
    void SetScene(int scene);
    void SetThreadCount(int iThread);
    void SetFilter(bool b);
    void SetInteractiveRender(bool b);
    void SetAo(bool b);
    void SetPathTracing(bool b);
    void SetMaxRayDepth(int maxDepth);
    void SetPathTracingDiffuseRayCount(int nbRays);
    void SetRayTracing(bool b);
    void SetAa(bool b);
    void SetAaFactor(int factor);
    void SetShadows(bool b)     ;
    void SetHardShadows(bool b)     ;
    void SetSoftShadows(bool b) ;
    void SetLightRadius(double radius) ;
    void SetLightSamples(int samples); 
   
private :
    void initControlWidget ();
    QActionGroup * actionGroup;
    QGroupBox * controlWidget;    //!< The Left dock group box.
    QGroupBox * projectGroupBox;  //!< The Right dock group box.
    QString currentDirectory;     
    GLViewer * viewer;            //!< The GLViewer (central window)

    QCheckBox * interactiveRenderCheckBox;
    QPushButton * rayButton;
    QComboBox * sceneComboBox;
    QCheckBox * focusCheckBox;
    QPushButton * saveButton;
    QPushButton * showButton;
    QCheckBox * wireframeCheckBox;
    QRadioButton * flatButton;
    QRadioButton *  smoothButton;
    QPushButton * snapshotButton;
                                

private Q_SLOTS:
    void rendererFinished ();
};

#endif // WINDOW_H


// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
