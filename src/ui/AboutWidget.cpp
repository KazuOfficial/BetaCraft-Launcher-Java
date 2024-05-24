#include "AboutWidget.h"
#include "Betacraft.h"

#include <QtWidgets>

#include "../core/Betacraft.h"
#include "../core/FileSystem.h"

AboutWidget::AboutWidget(QWidget *parent) : QWidget{parent} {
    initObjects();

    initAboutLayout();
    initLicenseLayout();
    initCreditsLayout();

    QFile fileCopying(":/COPYING.md");
    if (fileCopying.open(QIODevice::ReadOnly | QIODevice::Text)) {
        _licenseList->setMarkdown(fileCopying.readAll());
    }

    initMenu();
    initLayout();
}

void AboutWidget::initMenu() {
    _menu->addTab(_aboutSection, bc_translate("about_about_tab"));
    _menu->addTab(_creditsSection, bc_translate("about_credits_tab"));
    _menu->addTab(_licenseSection, bc_translate("about_license_tab"));
    _menu->setStyleSheet("QTabWidget::tab-bar { alignment: center; } "
                         "QTabWidget:pane { border-top: 1px solid gray; }");
}

void AboutWidget::initCreditsLayout() {
    _creditsList->setOpenExternalLinks(true);
    _creditsList->setHtml(getCreditsHtml());

    _creditsSectionLayout->setSpacing(0);
    _creditsSectionLayout->setContentsMargins(0, 0, 0, 0);
    _creditsSectionLayout->addWidget(_creditsList, 0, 0);

    _creditsSection->setLayout(_creditsSectionLayout);
}

void AboutWidget::initLicenseLayout() {
    _licenseSectionLayout->setSpacing(0);
    _licenseSectionLayout->setContentsMargins(0, 0, 0, 0);
    _licenseSectionLayout->addWidget(_licenseList, 0, 0);

    _licenseSection->setLayout(_licenseSectionLayout);
}

void AboutWidget::initAboutLayoutLogo() {
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(1);

    QPixmap pic(":/assets/logo.png");
    _logo->setPixmap(pic);
}

void AboutWidget::initAboutLayoutLinks() {
    _links->setText(getLinksHtml());
    _links->setTextFormat(Qt::RichText);
    _links->setTextInteractionFlags(Qt::TextBrowserInteraction);
    _links->setOpenExternalLinks(true);
}

void AboutWidget::initAboutLayout() {
    initAboutLayoutLogo();
    initAboutLayoutLinks();

    char *os = bc_file_os();
    QString versionString = QString("Version: %1 %2 %3")
                                .arg(BETACRAFT_VERSION)
                                .arg(QString(os))
                                .arg("master");

    free(os);

    _aboutSectionLayout->addWidget(_links, 0, 0, Qt::AlignCenter);
    _aboutSectionLayout->addWidget(new QLabel(versionString), 1, 0,
                                   Qt::AlignCenter);
    QString copyyear = QString("© Betacraft 2018-") + QString::number(QDate::currentDate().year());
    _aboutSectionLayout->addWidget(new QLabel(copyyear), 2, 0,
                                   Qt::AlignCenter);

    _aboutSectionLayout->setAlignment(Qt::AlignTop);
    _aboutSectionLayout->setSpacing(10);
    _aboutSectionLayout->setContentsMargins(10, 10, 10, 10);

    _aboutSection->setLayout(_aboutSectionLayout);
}

void AboutWidget::initLayout() {
    _layout->setRowMinimumHeight(0, 15);
    _layout->addWidget(_logo, 1, 1, Qt::AlignCenter | Qt::AlignTop);
    _layout->setRowMinimumHeight(2, 15);
    _layout->addWidget(_menu, 3, 1);

    _layout->setSpacing(0);
    _layout->setContentsMargins(5, 5, 5, 5);

    setLayout(_layout);
}

void AboutWidget::initObjects() {
    _layout = new QGridLayout(this);
    _menu = new QTabWidget(this);
    _logo = new QLabel(this);
    _links = new QLabel(this);
    _aboutSection = new QWidget(this);
    _creditsSection = new QWidget(this);
    _licenseSection = new QWidget(this);
    _aboutSectionLayout = new QGridLayout();
    _creditsSectionLayout = new QGridLayout();
    _licenseSectionLayout = new QGridLayout();
    _licenseList = new QTextBrowser(this);
    _creditsList = new QTextBrowser(this);
}

QString AboutWidget::getCreditsHtml() {
    QString output;
    QTextStream stream(&output);
    stream << "<center>\n";

    stream << "<h3>" << QObject::tr("Developers", "About Credits") << "</h3>\n";
    stream << "<p>Moresteck &lt;<a "
              "href='https://github.com/Moresteck'>https://github.com/"
              "Moresteck</a>&gt;</p>\n";
    stream << "<p>Kazu &lt;<a "
              "href='https://github.com/KazuOfficial'>https://github.com/"
              "KazuOfficial</a>&gt;</p>\n";
    stream << "<p>notdevcody &lt;<a "
              "href='https://github.com/notdevcody'>https://github.com/"
              "notdevcody</a>&gt;</p>\n";
    stream << "<h3>Translators</h3>\n";
    stream << "<p>Brazilian Portugese — Edvardvs &lt;<a "
              "href='https://github.com/Edvardvs'>https://github.com/Edvardvs</"
              "a>&gt;</p>\n";
    stream << "<p>Estonian — Zormein &lt;<a "
              "href='https://github.com/Zormein'>https://github.com/Zormein</"
              "a>&gt;</p>\n";
    stream << "<p>German — Don-Leandro &lt;<a "
              "href='https://github.com/Don-Leandro'>https://github.com/"
              "Don-Leandro</a>&gt;</p>\n";
    stream << "<p>Hungarian — TheClashFruit &lt;<a "
              "href='https://github.com/TheClashFruit'>https://github.com/"
              "TheClashFruit</a>&gt;</p>\n";
    stream << "<p>Italian — m-burani &lt;<a "
              "href='https://github.com/m-burani'>https://github.com/m-burani</"
              "a>&gt;</p>\n";
    stream << "<p>Kashubian, Kashubian (Zrzeszinka) — kaszebeman</p>\n";
    stream << "<p>Kashubian, Kashubian (Zrzeszinka) — Kaszëbskô Domôcëzna</p>\n";
    stream << "<p>Lithuanian — AshUosis &lt;<a "
              "href='https://github.com/AshUosis'>https://github.com/AshUosis</"
              "a>&gt;</p>\n";
    stream << "<p>Russian — TheEntropyShard &lt;<a "
              "href='https://github.com/TheEntropyShard'>https://github.com/"
              "TheEntropyShard</a>&gt;</p>\n";
    stream << "<p>Simplified Chinese, Traditional Chinese — xitieshiz2 &lt;<a "
              "href='https://github.com/xitieshiz2'>https://github.com/"
              "xitieshiz2</a>&gt;</p>\n";
    stream << "<p>Spanish — IOrbitSaturn &lt;<a "
              "href='https://github.com/IOrbitSaturn'>https://github.com/IOrbitSaturn</"
              "a>&gt;</p>\n";
    stream << "<p>Spanish — Nanrech &lt;<a "
              "href='https://github.com/Nanrech'>https://github.com/Nanrech</"
              "a>&gt;</p>\n";
    stream << "<p>Toki Pona — Waterrail &lt;<a "
              "href='https://github.com/Waterrail'>https://github.com/Waterrail</"
              "a>&gt;</p>\n";
    stream << "<p>Ukrainian — vil4ckc &lt;<a "
              "href='https://github.com/vil4ckc'>https://github.com/vil4ckc</"
              "a>&gt;</p>\n";

    stream << "</center>\n";

    return output;
}

QString AboutWidget::getLinksHtml() {
    QString output;
    QTextStream stream(&output);

    stream << "<a href=\"https://betacraft.uk/\">Website</a>&nbsp;&nbsp;";
    stream << "<a href=\"https://discord.gg/d4WvXeQ\">Discord</a>&nbsp;&nbsp;";
    stream << "<a "
              "href=\"https://github.com/betacraftuk/betacraft-launcher/"
              "\">GitHub</a>";

    return output;
}
