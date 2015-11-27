TEMPLATE = app
TARGET   = raymini
CONFIG  += qt opengl xml warn_on console release thread
QT *= opengl xml
HEADERS =   Window.h \
            GLViewer.h \
            QTUtils.h \
            Vertex.h \
            Triangle.h \
            Mesh.h \
            BoundingBox.h \
            Material.h \
            Object.h \
            Light.h \
            Scene.h \
            RayTracer.h \
            kd/KdTree.h \
            Ray.h \
            GuidedFilter.h \
            ParameterHandler.h \
            InteractiveRenderer.h \
            Surfel.h \
            kd/KdNode.h \
            kd/KdData.h \
            kd/KdIntersectionData.h \
            kd/KdPlane.h \
            kd/KdLeafNode.h \
            kd/KdMiddleNode.h \
            MathUtils.h \
            Vec3D.h \
            Pbgi.h \
            Edge.h \
            RadianceCalculator.h 

SOURCES =   Window.cpp \
            kd/KdMiddleNode.cpp \
            kd/KdLeafNode.cpp \
            GLViewer.cpp \
            QTUtils.cpp \
            Vertex.cpp \
            Triangle.cpp \
            Mesh.cpp \
            BoundingBox.cpp \
            Material.cpp \
            Object.cpp \
            Light.cpp \
            Scene.cpp \
            RayTracer.cpp \
            Ray.cpp \
            Main.cpp \
            GuidedFilter.cpp \
            ParameterHandler.cpp \
            Surfel.cpp \
            InteractiveRenderer.cpp \
            kd/KdPlane.cpp
          
DESTDIR=.

win32 {
    INCLUDEPATH += 'C:\\Qt\\Glew\\include'
    INCLUDEPATH += 'C:\\Qt\\Glut'
    INCLUDEPATH += 'C:\\Qt\\libQGLViewer'
    INCLUDEPATH += '.'    
    LIBS += -L"C:\\Qt\\libQGLViewer\\QGLViewer\\release" \
            -lQGLViewer2 \
            -L"C:\\Qt\\glew\\lib" -lglew32 \
            -static-libstdc++ \
            -llibgomp
#    LIBS += -L"C:\\Windows\\System32" \
#            -lQGLViewer2 \
#            -L"C:\\Windows\\System32" \
#            -lglew32 \
#            -static-libstdc++ \
#            -llibgomp
    QMAKE_CXXFLAGS += -O3 -fopenmp -std=c++0x
}
unix {
    QMAKE_CFLAGS   += -O3 -fopenmp
    QMAKE_CXXFLAGS += -O3 -std=c++0x -fopenmp
        LIBS += -lGLEW \
            -lQGLViewer \
            -lgomp \
            -lglut \
            -lm
}

MOC_DIR = .tmp
OBJECTS_DIR = .tmp

OTHER_FILES += \
    3D.rc

RC_FILE = \
    3D.rc
