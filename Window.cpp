#include <GL/glew.h>
#include "Window.h"
#include "ParameterHandler.h"
#include "RayTracer.h"
#include "InteractiveRenderer.h"

using namespace std;

/*!
 *  \brief  Creates the UI (upper menu, left and right dock and GLViewer)
 */
Window::Window () : QMainWindow (NULL) {
    try {
        viewer = new GLViewer;
    } catch (GLViewer::Exception e) {
        cerr << e.getMessage () << endl;
        exit (1);
    }
    setCentralWidget (viewer);


    /* Adding settings to upper menu */
    QMenu *settingsMenu = menuBar()->addMenu(tr("&Settings"));
    QAction *bgAct = new QAction(tr("&Background Color"), this);
    settingsMenu->addAction(bgAct);
    connect(bgAct, SIGNAL(triggered()),
            this, SLOT(setBGColor()));
    
    /* Adding quit and about buttons to upper menu */
    QMenu *fileMenu = menuBar()->addMenu(tr("&Help"));
    QAction *openAct = new QAction(tr("&About..."), this);
    fileMenu->addAction(openAct);
    connect(openAct, SIGNAL(triggered()),
            this, SLOT(about()));
    
    fileMenu->addSeparator();
    
    QAction *exitAct = new QAction(tr("E&xit"), this);
    fileMenu->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()),
            qApp, SLOT(closeAllWindows()));

    /* Defining size policy of the windows */
    QSizePolicy sizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    /* Left Doc: exection buttons */
    QDockWidget * controlDockWidget = new QDockWidget (this);
    initControlWidget ();
    controlDockWidget->setWidget (controlWidget);
    sizePolicy.setHeightForWidth(controlDockWidget->sizePolicy().hasHeightForWidth());
    controlDockWidget->setSizePolicy(sizePolicy);
    addDockWidget (Qt::LeftDockWidgetArea, controlDockWidget);
    controlDockWidget->setFeatures (QDockWidget::AllDockWidgetFeatures);
    statusBar()->showMessage("");

    /* Right Doc: parameters */
    QDockWidget * groupControlDockWidget = new QDockWidget (this);
    groupControlDockWidget->setWidget (projectGroupBox);
    groupControlDockWidget->adjustSize ();
    addDockWidget (Qt::RightDockWidgetArea, groupControlDockWidget);
    groupControlDockWidget->setFeatures (QDockWidget::AllDockWidgetFeatures);
}

Window::~Window () {

}

/*!
 *  \brief  Render the image
 */
void Window::renderRayImage () {
    qglviewer::Camera * cam = viewer->camera ();
    RayTracer * rayTracer = RayTracer::getInstance ();
    qglviewer::Vec p = cam->position ();
    qglviewer::Vec d = cam->viewDirection ();
    qglviewer::Vec u = cam->upVector ();
    qglviewer::Vec r = cam->rightVector ();
//    qglviewer::Vec p(0.0,0.0,0.0);
//    qglviewer::Vec d(0.0,0.0,-1.0);
//    qglviewer::Vec u(0.0,1.0,0.0);
//    qglviewer::Vec r (1.0,0.0,0.0);
    Vec3Df camPos (p[0], p[1], p[2]);
    Vec3Df viewDirection (d[0], d[1], d[2]);
    Vec3Df upVector (u[0], u[1], u[2]);
    Vec3Df rightVector (r[0], r[1], r[2]);
    float fieldOfView = cam->fieldOfView ();
    float aspectRatio = cam->aspectRatio ();
    unsigned int screenWidth = cam->screenWidth ();
    unsigned int screenHeight = cam->screenHeight ();
    QTime timer;
    timer.start ();


    viewer->setRayImage(rayTracer->render (camPos, viewDirection, upVector, rightVector,
                                           fieldOfView, aspectRatio, screenWidth, screenHeight));    //very long and consume resource, GUI freeze.



    statusBar()->showMessage(QString ("Raytracing performed in ") +
                             QString::number (timer.elapsed ()) +
                             QString ("ms at ") +
                             QString::number (screenWidth) + QString ("x") + QString::number (screenHeight) +
                             QString (" screen resolution"));
    viewer->setDisplayMode (GLViewer::RayDisplayMode);
}

/*!
 *  \brief  Signal callback used by interactive renderer to display the rendered image on the screen.
 */
void Window::rendererFinished () {
    InteractiveRenderer& renderer = RayTracer::getInstance()->getInterRenderer();

    //Managing interactive rendering data acces control: entering into the critical section
    renderer.lock();

        //Updating the image
        if (renderer.getImage())
            viewer->setRayImage( *renderer.getImage() );

        //Updating status bar
        statusBar()->showMessage(
            QString::number ( renderer.getFPS(), 'f', 2 ) +
            QString (" FPS, ") +
            QString::number (viewer->camera()->screenWidth()) + QString ("x") + QString::number (viewer->camera()->screenHeight()) +
                    (ParameterHandler::Instance()->GetInteractiveRender() ?
                         QString(", ") + renderer.getStatus() : QString(", stopped.")
                     )
        );

        //Managing GLViewer display mode in function of the interactivity mode switch value
        if (ParameterHandler::Instance()->GetInteractiveRender()) {
            viewer->setDisplayMode (GLViewer::RayDisplayMode);
        } else {
            renderer.disable();
            viewer->noAutoOpenGLDisplayMode = false;
        }

    //Leaving the critical section
    renderer.unlock();

    //If the interactive mode is still switched on, starting the process again.
    if (ParameterHandler::Instance()->GetInteractiveRender())
        renderer.start();
}

/*!
 *  \brief  Open a window for the selection of the background color
 */
void Window::setBGColor () {
    QColor c = QColorDialog::getColor (QColor (133, 152, 181), this);
    if (c.isValid () == true) {
        cout << c.red () << endl;
        RayTracer::getInstance ()->setBackgroundColor (Vec3Df (c.red (), c.green (), c.blue ()));
        viewer->setBackgroundColor (c);
        viewer->updateGL ();
    }
}

/*!
 *  \brief  Show the last rendered image
 */
void Window::showRayImage () {
    viewer->setDisplayMode (GLViewer::RayDisplayMode);
}

/*!
 *  \brief Save image of the GLViewer
 */
void Window::exportGLImage () {
    viewer->saveSnapshot (false, false);
}

/*!
 *  \brief  Save ray-traced image
 */
void Window::exportRayImage () {
    QString filename = QFileDialog::getSaveFileName (this,
                                                     "Save ray-traced image",
                                                     ".",
                                                     "*.jpg *.bmp *.png");
    if (!filename.isNull () && !filename.isEmpty ())
        viewer->getRayImage().save (filename);
}


/*!
 *  \brief  Show a content about this program 
 */
void Window::about () {
    QMessageBox::about (this, 
                        "About This Program", 
                        "<b>RayMini</b> <br> by <i>Tamy Boubekeur</i>.");
}

/*!
 *  \brief  Show a warning message for windows 
 *          systems and call interactive rendering depending on the answer
 *  \param  b State of the check box
 */
void Window::interactiveWarning (bool b) {
    bool option = false;
    
    /* If i'm activating interactive rendering show warning for windows systems*/
    if(b){
        
        int ret = QMessageBox::warning(this, tr("Windows Warning"),
                                       tr("The interactive rendering is not stable on windows systems.\n"
                                          "The program will problaby show a segmentation fault while executing.\n"
                                          "Do you want to continue?"),
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::Yes);
        
        switch (ret) {
        case QMessageBox::Yes:
            // Yes was clicked, execute the interactive code
            option = true;
            break;
        case QMessageBox::No:
            // No was clicked, set interactive check bool to false
            option = false;
            break;
        default:
            // should never be reached
            break;
        }
    }

    /* Update button states based on value selected by user*/
    interactiveRenderCheckBox  -> setChecked (option); 
    rayButton                  -> setDisabled(option); 
    sceneComboBox              -> setDisabled(option); 
    focusCheckBox              -> setDisabled(option); 
    saveButton                 -> setDisabled(option); 
    showButton                 -> setDisabled(option); 
    wireframeCheckBox          -> setDisabled(option); 
    flatButton                 -> setDisabled(option); 
    smoothButton               -> setDisabled(option); 
    snapshotButton             -> setDisabled(option); 
    SetInteractiveRender(option);
}

/*!
 *  \brief  Set the new scene selected from the box
 *  \param  scene Number of the scene
 */
void Window::SetScene(int scene)     {
    ParameterHandler* params = ParameterHandler::Instance();
    //TODO: fix interactivity problems
    params -> SetScene(scene);
    Scene::destroyInstance();
    viewer -> reset();
}

/*!
 *  \brief  Set the number of threads to be used by the program
 *  \param  iThread Number of threads used in the program
 */
void Window::SetThreadCount(int iThread)     {
    ParameterHandler* params = ParameterHandler::Instance();
    params -> SetThreadCount(iThread);
}

/*!
 *  \brief  Activate/Desactivate Focus effect
 *  \param  b Activate (true)/Desactivate (false) focus
 */
void Window::SetFilter(bool b){
    ParameterHandler* params = ParameterHandler::Instance();
    params -> SetFilter(b);
}

/*!
 *  \brief  Activate/Desactivate Interactive render
 *  \param  b Activate (true)/Desactivate (false) 
 */
void Window::SetInteractiveRender(bool b){
    ParameterHandler* params = ParameterHandler::Instance();
    InteractiveRenderer& renderer = RayTracer::getInstance()->getInterRenderer();

    params -> SetInteractiveRender(b);
    if (b) {
        connect(&renderer, SIGNAL (finished()), this, SLOT (rendererFinished()), Qt::UniqueConnection);
        renderer.begin(viewer);
    }
}

/*!
 *  \brief  Activate/Desactiva Ambient occlusion effect
 *  \param  b  Activate (true)/Desactivate (false) ambient occlusion
 */
void Window::SetAo(bool b){
    ParameterHandler* params = ParameterHandler::Instance();
    RESET_INTERACTIVITY_BEGIN;
    params -> SetAo(b);
    RESET_INTERACTIVITY_END;
}

/*!
 *  \brief  Activate/Desactiva Path tracing
 *  \param  b  Activate (true)/Desactivate (false) path tracing
 */
void Window::SetPathTracing(bool b){
    ParameterHandler* params = ParameterHandler::Instance();
    RESET_INTERACTIVITY_BEGIN;
    params -> SetPathTracing(b);
    RESET_INTERACTIVITY_END;
}

/*!
 *  \brief  Set the maximum reflections used on PathTracing
 *  \param  maxDepth value of maximun reflections of a ray
 */
void Window::SetMaxRayDepth(int maxDepth){
    ParameterHandler* params = ParameterHandler::Instance();
    RESET_INTERACTIVITY_BEGIN;
    params -> SetMaxRayDepth(maxDepth);
    RESET_INTERACTIVITY_END;
}

/*!
 *  \brief  Set the number of rays used on PathTracing
 *  \param  nbRays value of rays used
 */
void Window::SetPathTracingDiffuseRayCount( int nbRays){
    ParameterHandler* params = ParameterHandler::Instance();
    RESET_INTERACTIVITY_BEGIN;
    params -> SetPathTracingDiffuseRayCount((uint)nbRays);
    RESET_INTERACTIVITY_END;
}

/*!
 *  \brief  Activate/Desactivate RayTracing
 *  \param  b  Activate (true)/Desactivate (false) 
 */
void Window::SetRayTracing(bool b){
    ParameterHandler* params = ParameterHandler::Instance();
    RESET_INTERACTIVITY_BEGIN;
    params -> SetRayTracing(b);
    RESET_INTERACTIVITY_END;
}

/*!
 *  \brief  Activate/Desactiva Anti-aliasing effect
 *  \param  b  Activate (true)/Desactivate (false)
 */
void Window::SetAa(bool b){
    ParameterHandler* params = ParameterHandler::Instance();
    params -> SetAa(b);
}

/*!
 *  \brief  Set the anti-aliasing factor
 *  \param  index Index of the spinbox that will be converted into a value
 */
void Window::SetAaFactor(int index){
    ParameterHandler* params = ParameterHandler::Instance();
    unsigned short aaFactor = 2;
    
    if(index == 2 ){
        aaFactor = 4;
    }
    else if(index == 3){
        aaFactor = 8;
    }
    else if(index == 4){
        aaFactor = 16;
    }

    params -> SetAaFactor(aaFactor);
}

/*!
 *  \brief  Activate/Desactivate Shadows effect
 *  \param  b  Activate (true)/Desactivate (false) 
 */
void Window::SetShadows(bool b)     {
    ParameterHandler* params = ParameterHandler::Instance();
    RESET_INTERACTIVITY_BEGIN;
    params -> SetShadows(b);
    RESET_INTERACTIVITY_END;
}

/*!
 *  \brief  Activate/Desactivate Hard shadows effect
 *  \param  b  Activate (true)/Desactivate (false) 
 */
void Window::SetHardShadows(bool b) {
    ParameterHandler* params = ParameterHandler::Instance();
    RESET_INTERACTIVITY_BEGIN;
    params -> SetHardShadows(b);
    RESET_INTERACTIVITY_END;
}

/*!
 *  \brief  Activate/Desactivate Soft Shadows effect
 *  \param  b  Activate (true)/Desactivate (false) 
 */
void Window::SetSoftShadows(bool b) {
    ParameterHandler* params = ParameterHandler::Instance();
    RESET_INTERACTIVITY_BEGIN;
    params -> SetSoftShadows(b);
    RESET_INTERACTIVITY_END;
}

/*!
 *  \brief  Set the length of the radius used on a area light
 *  \param  radius  Length of the radius of a area light 
 */
void Window::SetLightRadius(double radius) {
    ParameterHandler* params = ParameterHandler::Instance();
    RESET_INTERACTIVITY_BEGIN;
    params -> SetLightRadius(radius);
    RESET_INTERACTIVITY_END;
}

/*!
 *  \brief  Set the number of samples to be considered from an area light
 *  \param  samples Number of samples of light used on Soft Shadows algorithm
 */
void Window::SetLightSamples(int samples){
    ParameterHandler* params = ParameterHandler::Instance();
    RESET_INTERACTIVITY_BEGIN;
    params -> SetLightSamples((uint)samples);
    RESET_INTERACTIVITY_END;
}

void Window::initControlWidget () {

    /* Defining size policy of the windows */
    QSizePolicy sizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    controlWidget = new QGroupBox ();
    QVBoxLayout * layout = new QVBoxLayout (controlWidget);
    QGroupBox * previewGroupBox = new QGroupBox ("Preview", controlWidget);
    QVBoxLayout * previewLayout = new QVBoxLayout (previewGroupBox);
    
    wireframeCheckBox = new QCheckBox ("Wireframe", previewGroupBox);
    connect (wireframeCheckBox, SIGNAL (toggled (bool)), viewer, SLOT (setWireframe (bool)));
    previewLayout->addWidget (wireframeCheckBox);
   
    QButtonGroup * modeButtonGroup = new QButtonGroup (previewGroupBox);
    modeButtonGroup->setExclusive (true);
    flatButton = new QRadioButton ("Flat", previewGroupBox);
    smoothButton = new QRadioButton ("Smooth", previewGroupBox);
    modeButtonGroup->addButton (flatButton, static_cast<int>(GLViewer::Flat));
    modeButtonGroup->addButton (smoothButton, static_cast<int>(GLViewer::Smooth));
    connect (modeButtonGroup, SIGNAL (buttonClicked (int)), viewer, SLOT (setRenderingMode (int)));
    previewLayout->addWidget (flatButton);
    previewLayout->addWidget (smoothButton);
    
    snapshotButton  = new QPushButton ("Save preview", previewGroupBox);
    connect (snapshotButton, SIGNAL (clicked ()) , this, SLOT (exportGLImage ()));
    previewLayout->addWidget (snapshotButton);

    previewLayout->addStretch (0);

    layout->addWidget (previewGroupBox);
    
    QGroupBox * rayGroupBox = new QGroupBox ("Ray Tracing", controlWidget);
    QVBoxLayout * rayLayout = new QVBoxLayout (rayGroupBox);
    rayButton = new QPushButton ("Render", rayGroupBox);
    connect (rayButton, SIGNAL (clicked ()), this, SLOT (renderRayImage ()));

    /* Interactive rendering*/
    ParameterHandler* params = ParameterHandler::Instance(); // Get initial values
    interactiveRenderCheckBox = new QCheckBox ("Interactive", rayGroupBox);
    interactiveRenderCheckBox -> setChecked (params->GetInteractiveRender() );


    showButton = new QPushButton ("Show", rayGroupBox);
    connect (showButton, SIGNAL (clicked ()), this, SLOT (showRayImage ()));
    saveButton  = new QPushButton ("Save", rayGroupBox);
    connect (saveButton, SIGNAL (clicked ()) , this, SLOT (exportRayImage ()));

    /* Adding Widgets to layout */
    rayLayout->addWidget (interactiveRenderCheckBox);
    rayLayout->addWidget (rayButton);
    rayLayout->addWidget (showButton);
    rayLayout->addWidget (saveButton);

    /*Adding layout to interface*/
    layout->addWidget (rayGroupBox);
    
    /* ==== Program parameters deck ==== */
    projectGroupBox = new QGroupBox ();
    projectGroupBox->setSizePolicy(sizePolicy);    

    QVBoxLayout * projectLayout = new QVBoxLayout (projectGroupBox);

    /* ==== General Parameters Selection ==== */
    QGroupBox * generalGroupBox = new QGroupBox ("General", projectGroupBox);
    generalGroupBox->setSizePolicy(sizePolicy);    

    QVBoxLayout * generalLayout = new QVBoxLayout (generalGroupBox);

    QSpinBox * threadsSpinBox = new  QSpinBox (generalGroupBox);
    threadsSpinBox -> setFixedSize(80,20);
    threadsSpinBox -> setRange(1,16);
    threadsSpinBox -> setValue(params -> GetThreadCount());
    connect (threadsSpinBox, SIGNAL (valueChanged(int)), this, SLOT (SetThreadCount(int)));
    
    QLabel      * threadsLabel;
    threadsLabel = new QLabel(tr("Threads:"));
    threadsLabel -> setBuddy(threadsSpinBox);

    sceneComboBox = new QComboBox (generalGroupBox);
    sceneComboBox -> addItem(tr("Default"));
    sceneComboBox -> addItem(tr("Spheres"));
    sceneComboBox -> addItem(tr("Box"));
    sceneComboBox -> addItem(tr("BMW"));
    sceneComboBox -> setFixedSize(80,20);
    connect (sceneComboBox, SIGNAL (currentIndexChanged(int)), this, SLOT (SetScene (int)));

    QLabel      * sceneLabel;
    sceneLabel = new QLabel(tr("Scene:"));
    sceneLabel -> setBuddy(sceneComboBox);

    focusCheckBox = new QCheckBox ("Effect Focus", generalGroupBox);
    focusCheckBox -> setChecked (params->GetFilter() );
    connect (focusCheckBox, SIGNAL (toggled (bool)), this, SLOT (SetFilter(bool)));
   
    /* Creating tables for general parameters */
    QWidget *generalLayoutWidget = new QWidget(generalGroupBox);
    QFormLayout *generalFormLayout = new QFormLayout(generalLayoutWidget);
    generalFormLayout -> setContentsMargins(0, 0, 0, 0);
    generalFormLayout -> setWidget(0, QFormLayout::LabelRole, sceneLabel);
    generalFormLayout -> setWidget(0, QFormLayout::FieldRole, sceneComboBox);
    generalFormLayout -> setWidget(1, QFormLayout::LabelRole, threadsLabel);
    generalFormLayout -> setWidget(1, QFormLayout::FieldRole, threadsSpinBox);
    generalFormLayout -> setWidget(2, QFormLayout::SpanningRole, focusCheckBox);

    /* Adding widget to layout */
    generalLayout->addWidget (generalLayoutWidget);

    /* == Section of Anti Aliasing parameters == */
    QGroupBox * aaGroupBox = new QGroupBox ("Anti Aliasing",projectGroupBox);
    QVBoxLayout * aaLayout = new QVBoxLayout (aaGroupBox);

    QCheckBox * aaCheckBox = new QCheckBox ("Enable", aaGroupBox);
    aaCheckBox -> setChecked (params->GetAa() );
    connect (aaCheckBox, SIGNAL (toggled (bool)), this, SLOT (SetAa(bool)));

    QComboBox * aaFactorComboBox = new QComboBox (aaGroupBox);
    aaFactorComboBox -> addItem(tr("2"));
    aaFactorComboBox -> addItem(tr("4"));
    aaFactorComboBox -> addItem(tr("8"));
    aaFactorComboBox -> addItem(tr("16"));
    aaFactorComboBox -> setFixedSize(80,20);
    
    int index = 1, aaFactor = params -> GetAaFactor();
    if(aaFactor == 4){
        index = 2;
    }
    else if(aaFactor == 8){
        index = 3;
    }
    else if(aaFactor == 16){
        index = 4;
    }
    
    aaFactorComboBox -> setCurrentIndex(index);
    connect (aaFactorComboBox, SIGNAL (currentIndexChanged(int)), this, SLOT (SetAaFactor (int)));

    QWidget *aaLayoutWidget = new QWidget(aaGroupBox);
    QFormLayout *aaFormLayout = new QFormLayout(aaLayoutWidget);;
    QLabel *aaLabel = new QLabel(tr("Factor:"));
    aaLabel    -> setBuddy(aaFactorComboBox);
    aaFormLayout -> addWidget(aaLabel);
    aaFormLayout -> addWidget(aaFactorComboBox);
    aaFormLayout -> setContentsMargins(0, 0, 0, 0);
    aaFormLayout -> setWidget(0, QFormLayout::LabelRole, aaLabel);
    aaFormLayout -> setWidget(0, QFormLayout::FieldRole, aaFactorComboBox);

    /* Adding widget to UI */
    aaLayout -> addWidget (aaCheckBox);
    aaLayout -> addWidget (aaLayoutWidget);

    /* == Section of Shadows parameters == */
    QGroupBox * shadowsGroupBox = new QGroupBox ("Shadows",projectGroupBox);
    QVBoxLayout * shadowsLayout = new QVBoxLayout (shadowsGroupBox);

    QCheckBox * shadowsCheckBox = new QCheckBox ("Enable",shadowsGroupBox);
    shadowsCheckBox -> setChecked(params->GetShadows());

    QRadioButton * hardShadowsRB= new QRadioButton("Hard shadows",shadowsGroupBox);
    hardShadowsRB -> setChecked(params->GetHardShadows());

    QRadioButton * softShadowsRB = new QRadioButton("Soft shadows", shadowsGroupBox);
    softShadowsRB -> setChecked(params->GetSoftShadows());

    /* Creating control buttons for light control used on softShadows*/
    QDoubleSpinBox * lightRadiusSpinBox = new  QDoubleSpinBox (shadowsGroupBox);
    lightRadiusSpinBox -> setFixedSize(80,20);
    lightRadiusSpinBox -> setRange(0,10);
    lightRadiusSpinBox -> setValue(params -> GetLightRadius());
    
    QWidget * lightRadiusLayoutWidget = new QWidget(shadowsGroupBox);
    QLabel  * lightRadiusLabel;
    lightRadiusLabel = new QLabel(tr("Radius:"));
    lightRadiusLabel  -> setBuddy(lightRadiusSpinBox);

    QSpinBox * lightSamplesSpinBox = new  QSpinBox (shadowsGroupBox);
    lightSamplesSpinBox -> setFixedSize(80,20);
    lightSamplesSpinBox -> setRange(1,40);
    lightSamplesSpinBox -> setValue(params -> GetLightSamples());
    
    QLabel * lightSamplesLabel;
    lightSamplesLabel = new QLabel(tr("Samples:"));
    lightSamplesLabel -> setBuddy(lightRadiusSpinBox);

    /* Creating table for light control used on softShadows */
    QFormLayout *shadowsFormLayout = new QFormLayout(lightRadiusLayoutWidget);
    shadowsFormLayout -> setContentsMargins(0, 0, 0, 0);
    shadowsFormLayout -> setWidget(0, QFormLayout::LabelRole, lightRadiusLabel);
    shadowsFormLayout -> setWidget(0, QFormLayout::FieldRole, lightRadiusSpinBox);
    shadowsFormLayout -> setWidget(1, QFormLayout::LabelRole, lightSamplesLabel);
    shadowsFormLayout -> setWidget(1, QFormLayout::FieldRole, lightSamplesSpinBox);

    /* Button's actions */
    connect (shadowsCheckBox, SIGNAL (toggled (bool)), this, SLOT (SetShadows(bool)));
    connect (hardShadowsRB,   SIGNAL (toggled (bool)), this, SLOT (SetHardShadows(bool)));
    connect (softShadowsRB,   SIGNAL (toggled (bool)), this, SLOT (SetSoftShadows(bool)));
    connect (shadowsCheckBox, SIGNAL (toggled (bool)), hardShadowsRB, SLOT (setVisible(bool)));
    connect (shadowsCheckBox, SIGNAL (toggled (bool)), softShadowsRB, SLOT (setVisible(bool)));
    connect (lightRadiusSpinBox, SIGNAL (valueChanged(double)), this, SLOT (SetLightRadius(double)));
    connect (lightSamplesSpinBox, SIGNAL (valueChanged(int)), this, SLOT (SetLightSamples(int)));

    /* Adding widget to UI */
    shadowsLayout -> addWidget (shadowsCheckBox);
    shadowsLayout -> addWidget (hardShadowsRB);
    shadowsLayout -> addWidget (softShadowsRB);
    shadowsLayout -> addWidget (lightRadiusLayoutWidget);

    /* == Section of illumination parameters == */
    QGroupBox * raysGroupBox = new QGroupBox ("Illumination",projectGroupBox);
    QVBoxLayout * raysLayout = new QVBoxLayout (raysGroupBox);

    /* Select betwenn Ray tracing and Path Tracing*/
    QRadioButton * rayTracingRadioButton = new QRadioButton ("Direct Illumination", raysGroupBox);
    rayTracingRadioButton -> setChecked( params -> GetRayTracing());
    connect (rayTracingRadioButton, SIGNAL (toggled (bool)), this, SLOT (SetRayTracing(bool)));
    
    QRadioButton * pathTracingRadioButton = new QRadioButton ("Path Tracing", raysGroupBox);
    pathTracingRadioButton -> setChecked ( params -> GetPathTracing());
    connect (pathTracingRadioButton, SIGNAL (toggled (bool)), this, SLOT (SetPathTracing (bool)));

    /* Define parameters for Path Tracing (if path tracing is enable)*/
    QSpinBox * maxRayDepthSpinBox = new  QSpinBox (raysGroupBox);
    maxRayDepthSpinBox -> setValue( params -> GetMaxRayDepth());
    maxRayDepthSpinBox -> setFixedSize(70,20);
    connect (maxRayDepthSpinBox, SIGNAL (valueChanged(int)), this, SLOT (SetMaxRayDepth (int)));

    QLabel * maxRayDepthLabel;
    maxRayDepthLabel = new QLabel(tr("Max Ray Depth:"));
    maxRayDepthLabel -> setBuddy(maxRayDepthSpinBox);

    QSpinBox * pathTracingDiffuseRaySB = new  QSpinBox (raysGroupBox);
    pathTracingDiffuseRaySB -> setValue( params -> GetPathTracingDiffuseRayCount());
    pathTracingDiffuseRaySB -> setFixedSize(70,20);
    pathTracingDiffuseRaySB -> setRange(5,500);
    connect (pathTracingDiffuseRaySB, SIGNAL (valueChanged(int)), this, SLOT (SetPathTracingDiffuseRayCount(int)));
    
    QLabel * pathTracingDiffuseRayLabel;
    pathTracingDiffuseRayLabel = new QLabel(tr("Diffuse rays:"));
    pathTracingDiffuseRayLabel -> setBuddy(maxRayDepthSpinBox);

    /* Creating table for path tracing parameters*/
    QWidget *pathTracingLayoutWidget = new QWidget(raysGroupBox);
    QFormLayout *pathTracingLayout = new QFormLayout(pathTracingLayoutWidget);;
    pathTracingLayout -> setContentsMargins(0, 0, 0, 0);
    pathTracingLayout -> setWidget(0, QFormLayout::LabelRole, maxRayDepthLabel);
    pathTracingLayout -> setWidget(0, QFormLayout::FieldRole, maxRayDepthSpinBox);
    pathTracingLayout -> setWidget(1, QFormLayout::LabelRole, pathTracingDiffuseRayLabel);
    pathTracingLayout -> setWidget(1, QFormLayout::FieldRole, pathTracingDiffuseRaySB);

    /* Ambient Occlusion parameters */
    QCheckBox * aoCheckBox = new QCheckBox ("Ambient Occlusion", raysGroupBox);
    aoCheckBox->setChecked(params->GetAo());
    connect (aoCheckBox, SIGNAL (toggled (bool)), this, SLOT (SetAo(bool)));

    /* Adding widget to UI */
    raysLayout -> addWidget (rayTracingRadioButton);
    raysLayout -> addWidget (pathTracingRadioButton);
    raysLayout -> addWidget (pathTracingLayoutWidget);
    raysLayout -> addWidget (aoCheckBox);

    /* == Interactive rendering ==
       Disabling buttons 
    */
    connect(interactiveRenderCheckBox, SIGNAL(toggled(bool)), rayButton, SLOT(setDisabled(bool)));
    connect(interactiveRenderCheckBox, SIGNAL(toggled(bool)), sceneComboBox, SLOT(setDisabled(bool)));
    connect(interactiveRenderCheckBox, SIGNAL(toggled(bool)), focusCheckBox, SLOT(setDisabled(bool)));
    connect(interactiveRenderCheckBox, SIGNAL(toggled(bool)), saveButton, SLOT(setDisabled(bool)));
    connect(interactiveRenderCheckBox, SIGNAL(toggled(bool)), showButton, SLOT(setDisabled(bool)));
    connect(interactiveRenderCheckBox, SIGNAL(toggled(bool)), wireframeCheckBox, SLOT(setDisabled(bool)));
    connect(interactiveRenderCheckBox, SIGNAL(toggled(bool)), flatButton, SLOT(setDisabled(bool)));
    connect(interactiveRenderCheckBox, SIGNAL(toggled(bool)), smoothButton, SLOT(setDisabled(bool)));
    connect(interactiveRenderCheckBox, SIGNAL(toggled(bool)), snapshotButton, SLOT(setDisabled(bool)));


#ifdef Q_OS_WIN /* Show warning on windows*/
    connect(interactiveRenderCheckBox, SIGNAL(toggled(bool)), this, SLOT (interactiveWarning(bool)));
#endif
    
#ifndef Q_OS_WIN /* Working on linux*/
    connect(interactiveRenderCheckBox, SIGNAL(toggled(bool)), this, SLOT (SetInteractiveRender(bool)));
#endif

    /* == Adding groups to deck == */
    projectLayout -> addWidget (generalGroupBox);
    projectLayout -> addWidget (aaGroupBox);
    projectLayout -> addWidget (shadowsGroupBox);
    projectLayout -> addWidget (raysGroupBox);

    layout -> addStretch (0);
}
