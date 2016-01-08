#include <QDebug>
#include "panel.hpp"
#include "ui_panel.h"
#include "picturebutton.hpp"
#include "abstractmodule.hpp"
#include "moduleregistry.hpp"
#include "coorimpl.hpp"

Panel::Panel(QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent, f),
    ui(new Ui::Panel)
{
    ui->setupUi(this);

    int icon_space_width = 120;
    int icon_space_height = 180;
    int icon_width = 80;
    int icon_height = 80;
    int icon_space_margin = 10;
    int row_num = 1;
    PictureButton *icon = 0;
    quint8 num = 0;
    ModuleRegistry *pModuleReg(ModuleRegistry::getRegistry());
    ModuleRegistry::Iterator iter;
    for (iter = pModuleReg->begin(); iter != pModuleReg->end(); iter++) {
        CoorImpl *pCoor = *iter;
        AbstractModule *pModule = pCoor->getModule();
        QPixmap pixmap;
        pModule->getImage(pixmap);
        icon = new PictureButton(pixmap, this);
        if (0 != pCoor) {
            connect(icon, SIGNAL(clicked()), pCoor, SLOT(onActivate()));
            connect(pCoor, SIGNAL(connected()), icon, SLOT(enabled()));
            connect(pCoor, SIGNAL(disconnected()), icon, SLOT(disabled()));
        }
        int column = num / row_num;
        int row = num % row_num;
        icon->setGeometry(icon_space_width * column + icon_space_margin,
                          icon_space_height * row + icon_space_margin,
                          icon_width,
                          icon_height);
        num++;
    }
}

Panel::~Panel()
{
    delete ui;
}
