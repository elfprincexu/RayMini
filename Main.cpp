#include <QCoreApplication>

#include <QApplication>
#include "Window.h"
#include <QDir>
#include <QPixmap>
#include <QSplashScreen>
//#include <QPlastiqueStyle>
//#include <QCleanlooksStyle>
#include <QStyleFactory>
#include <string>
#include <iostream>

#include "QTUtils.h"

using namespace std;


/*! \mainpage Raymini Documentation
 *
 * \section intro_sec Introduction
 * RayMini is a minimal raytracer implemented in C++/OpenGL/Qt.
 * This software package is meants to be distributed to Telecom ParisTech student only.
 * \section install_sec Installation
 *
 * \subsection tools_subsec Tools required:
 * <ul>  
 * <li>GCC >v4 </li>
 * <li> OpenGL </li>
 * <li> QT >v4.4 </li>
 * <li> libQGLViewer </li>
 * <li> GLEW (for GPU extensions) </li>
 * </ul>
 * <VAR> 
 * Edit the file raymini.pro to adapt it to your configuration (has been tested under Linux Ubuntu, Linux Fedora and Win7).
 * </VAR>
 *
 * \subsection running Running the program
 * - On linux: 
 *    -# qmake-qt4 raymini.pro
 *    -# make
 *    -# ./raymini
 * 
 *
 * - On windows:
 *    <BR> Use QT Creator and do not forget to setup the run configuration properly so that the raymini directory is the working directory. 
 *    <BR> Otherwise, the 'models' directory will not be found by the raymini executable.
 *    <BR> If you want to run the executable directly (by double-clikcing on it in the Windows file explorer), copy the model directory into the release directory first 
 *     and double-click on raymini.exe. 
 *
 * \section authors Authors
 *  - Tamy Boubekeur    (tamy.boubekeur@telecom-paristech.fr)
 *  - Junxian Xu        (junxian.xu@telecom-paristech.fr)
 *  - Maxim Karpushin   (maxim.karpushin@telecom-paristech.fr)
 *  - Tiago C. Silva    (tiago.silva@telecom-paristech.fr)
 *  - Vinicius Gardelli (vinicius.gardelli@telecom-paristech.fr)
 */
int main (int argc, char **argv)
{

  try {
  QApplication raymini (argc, argv);

  setBoubekQTStyle (raymini);
  QApplication::setStyle (QStyleFactory::create("fusion"));
  raymini.setAttribute(Qt::AA_DontUseNativeMenuBar,true);

  Window * window = new Window ();
  window->setWindowTitle ("RayMini: A minimal raytracer.");
  window->show();
  raymini.connect (&raymini, SIGNAL (lastWindowClosed()), &raymini, SLOT (quit()));

  return raymini.exec ();
  }
  catch (exception e)
  {
    cerr << e.what() << endl;
    //exit (1);
  }
  catch (Mesh::Exception e)
  {
    cerr << e.getMessage() << endl;
    //exit (1);
  }
}

