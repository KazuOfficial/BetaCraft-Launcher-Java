#include "InstanceEditAppearanceWidget.h"

#include "../Betacraft.h"
#include <QtWidgets>

QString icon_path;

InstanceEditAppearanceWidget::InstanceEditAppearanceWidget(QWidget *parent)
    : QWidget{parent} {
    _layout = new QGridLayout(this);
    _instanceNameGroup = new QGroupBox(this);
    _instanceNameTextbox = new QLineEdit(this);
    _showGameLogCheckbox =
        new QCheckBox(bc_translate("instance_show_game_log"), this);
    _keepOpenCheckbox =
        new QCheckBox(bc_translate("instance_keep_launcher_open"), this);
    _instanceIconGroup = new QGroupBox(this);
    _instanceIcon = new QLabel(this);
    _instanceIconBrowseButton = new QPushButton(this);
    _instanceIconDefaultButton = new QPushButton(this);
    _instanceGameGroup = new QGroupBox(this);
    _instanceGameWidthLabel = new QLabel(this);
    _instanceGameHeightLabel = new QLabel(this);
    _instanceGameWidthTextbox = new QLineEdit(this);
    _instanceGameHeightTextbox = new QLineEdit(this);
    _instanceGameMaximizedCheckbox = new QCheckBox(this);
    _instanceGameFullscreenCheckbox = new QCheckBox(this);
    _instanceNameLayout = new QGridLayout();
    _instanceIconLayout = new QGridLayout();
    _instanceGameLayout = new QGridLayout();
    _instanceCheckboxLayout = new QGridLayout();
    _instanceCheckboxGroup = new QGroupBox(this);

    _instanceNameGroup->setTitle(bc_translate("instance_name"));
    _instanceIconGroup->setTitle(bc_translate("instance_icon_category"));
    _instanceIconBrowseButton->setText(bc_translate("instance_icon_browse"));
    _instanceIconDefaultButton->setText(bc_translate("instance_icon_revert"));
    _instanceGameGroup->setTitle(bc_translate("instance_game_category"));
    _instanceGameWidthLabel->setText(bc_translate("instance_game_width"));
    _instanceGameHeightLabel->setText(bc_translate("instance_game_height"));
    _instanceGameMaximizedCheckbox->setText(
        bc_translate("instance_game_resolution_maximized"));
    _instanceGameFullscreenCheckbox->setText(
        bc_translate("instance_game_fullscreen"));

    _instanceNameLayout->addWidget(_instanceNameTextbox);
    _instanceNameGroup->setLayout(_instanceNameLayout);

    _instanceCheckboxLayout->addWidget(_showGameLogCheckbox);
    _instanceCheckboxLayout->addWidget(_keepOpenCheckbox);
    _instanceCheckboxGroup->setLayout(_instanceCheckboxLayout);

    _instanceIcon->setFixedSize(64, 64);
    _instanceIconLayout->addWidget(_instanceIcon, 0, 0, 1, 1);
    _instanceIconLayout->addWidget(_instanceIconBrowseButton, 0, 1, 1, 1);
    _instanceIconLayout->addWidget(_instanceIconDefaultButton, 0, 2, 1, 1);
    _instanceIconGroup->setLayout(_instanceIconLayout);

    _instanceGameMaximizedCheckbox->setDisabled(1);
    _instanceGameFullscreenCheckbox->setDisabled(1);

    _instanceGameLayout->addWidget(_instanceGameWidthLabel, 0, 0, 1, 1);
    _instanceGameLayout->addWidget(_instanceGameHeightLabel, 1, 0, 1, 1);
    _instanceGameLayout->addWidget(_instanceGameWidthTextbox, 0, 1, 1, 1);
    _instanceGameLayout->addWidget(_instanceGameHeightTextbox, 1, 1, 1, 1);
    _instanceGameLayout->addWidget(_instanceGameMaximizedCheckbox, 2, 0, 1, 1);
    _instanceGameLayout->addWidget(_instanceGameFullscreenCheckbox, 3, 0, 1, 1);
    _instanceGameGroup->setLayout(_instanceGameLayout);

    _layout->addWidget(_instanceNameGroup, 0, 0, 1, 11);
    _layout->addWidget(_instanceCheckboxGroup, 1, 0, 1, 11);
    _layout->addWidget(_instanceIconGroup, 2, 0, 1, 11);
    _layout->addWidget(_instanceGameGroup, 3, 0, 1, 11);

    _layout->setAlignment(Qt::AlignVCenter);

    _layout->setSpacing(5);
    _layout->setContentsMargins(10, 10, 10, 10);

    setStyleSheet("QLabel { font-size: 14px; }");
    setLayout(_layout);

    connect(_instanceIconBrowseButton, SIGNAL(released()), this,
            SLOT(onBrowseButtonClicked()));
    connect(_instanceIconDefaultButton, SIGNAL(released()), this,
            SLOT(onRevertDefaultButtonClicked()));
}

void InstanceEditAppearanceWidget::onRevertDefaultButtonClicked() {
    if (QFile::exists(icon_path)) {
        remove(icon_path.toStdString().c_str());
        _instanceIcon->setPixmap(QPixmap(":/assets/unknown_pack.png")
                                     .scaled(_instanceIcon->width(),
                                             _instanceIcon->height(),
                                             Qt::KeepAspectRatio));
    }
}

void InstanceEditAppearanceWidget::onBrowseButtonClicked() {
    QString path = QFileDialog::getOpenFileName(
        this, tr("Select an icon"), "/", tr("Image Files (*.png, *.jpg )"));

    if (!path.isNull()) {
        QImage img(path);
        QImage scaledImage = img.scaled(256, 256, Qt::KeepAspectRatio);
        scaledImage.save(icon_path, "PNG", 80);

        _instanceIcon->setPixmap(QPixmap(icon_path).scaled(
            _instanceIcon->width(), _instanceIcon->height(),
            Qt::KeepAspectRatio));
    }
}

void InstanceEditAppearanceWidget::setInstance(bc_instance instance) {
    _instanceNameTextbox->setText(instance.name);
    _instanceGameWidthTextbox->setText(
        QString::fromStdString(std::to_string(instance.width)));
    _instanceGameHeightTextbox->setText(
        QString::fromStdString(std::to_string(instance.height)));
    _instanceGameMaximizedCheckbox->setChecked(instance.maximized);
    _instanceGameFullscreenCheckbox->setChecked(instance.fullscreen);
    _showGameLogCheckbox->setChecked(instance.show_log);
    _keepOpenCheckbox->setChecked(instance.keep_open);

    icon_path = QString("%1%2")
                    .arg(QString(instance.path).split("bc_instance.json")[0])
                    .arg("instance_icon.png");

    QString icon = ":/assets/unknown_pack.png";
    if (QFile::exists(icon_path)) {
        icon = icon_path;
    }

    _instanceIcon->setPixmap(QPixmap(icon).scaled(
        _instanceIcon->width(), _instanceIcon->height(), Qt::KeepAspectRatio));
}

bc_instance *InstanceEditAppearanceWidget::getSettings() {
    bc_instance *instance = new bc_instance();

    snprintf(instance->name, sizeof(instance->name), "%s",
             _instanceNameTextbox->text().toStdString().c_str());
    instance->width = _instanceGameWidthTextbox->text().toInt();
    instance->height = _instanceGameHeightTextbox->text().toInt();
    instance->maximized = _instanceGameMaximizedCheckbox->isChecked();
    instance->fullscreen = _instanceGameFullscreenCheckbox->isChecked();
    instance->show_log = _showGameLogCheckbox->isChecked();
    instance->keep_open = _keepOpenCheckbox->isChecked();

    return instance;
}
