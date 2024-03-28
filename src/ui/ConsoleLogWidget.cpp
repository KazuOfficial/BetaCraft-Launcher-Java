#include "ConsoleLogWidget.h"
#include "Betacraft.h"

#include "../core/ProcessHandler.h"

#include <QtWidgets>

QString _consoleLogString;
int _consoleLogClearCounter = 0;
bool _paused = false;

ConsoleLogWidget::ConsoleLogWidget(QWidget *parent) : QWidget(parent) {
    initObjects();
    initLayout();
    connectSignalsToSlots();
}

void ConsoleLogWidget::connectSignalsToSlots() {
    connect(_consoleLogTimer, SIGNAL(timeout()), this,
            SLOT(UpdateConsoleLog()));
    connect(_copyButton, &QPushButton::released, this, [this]() {
        QApplication::clipboard()->setText(_consoleLogString);
    });
    connect(_clearButton, &QPushButton::released, this,
            &ConsoleLogWidget::clearConsoleLog);
    connect(_pauseButton, &QPushButton::released, this,
            &ConsoleLogWidget::pauseConsoleLog);
}

void ConsoleLogWidget::clearConsoleLog() {
    _consoleLog->setText("");
    _consoleLogString = "";
    _consoleLogClearCounter = strlen(bc_process_log);
}

void ConsoleLogWidget::pauseConsoleLog() {
    _paused = !_paused;
    _paused ? _pauseButton->setText(bc_translate("gamelog_unpause"))
            : _pauseButton->setText(bc_translate("gamelog_pause"));
}

void ConsoleLogWidget::initLayout() {
    _consoleLog->setReadOnly(true);
    _searchBar->setPlaceholderText(bc_translate("general_search_placeholder"));

    setWindowTitle(bc_translate("gamelog_window_title"));

    resize(600, 450);
    setMinimumSize(600, 450);

    _layout->addWidget(_searchBar, 0, 0, 1, 1);
    _layout->addWidget(_copyButton, 0, 2, 1, 1);
    _layout->addWidget(_pauseButton, 0, 3, 1, 1);
    _layout->addWidget(_clearButton, 0, 4, 1, 1);
    _layout->addWidget(_consoleLog, 1, 0, 1, 11);

    _layout->setSpacing(0);
    _layout->setContentsMargins(5, 5, 5, 5);

    setLayout(_layout);
}

void ConsoleLogWidget::initObjects() {
    _layout = new QGridLayout(this);
    _consoleLog = new QTextEdit(this);
    _consoleLogTimer = new QTimer(this);
    _searchBar = new QLineEdit(this);
    _clearButton = new QPushButton(bc_translate("gamelog_clear"), this);
    _pauseButton = new QPushButton(bc_translate("gamelog_pause"), this);
    _copyButton = new QPushButton(bc_translate("gamelog_copy"), this);
}

void ConsoleLogWidget::UpdateConsoleLog() {
    QString log(bc_process_log);
    log.remove(0, _consoleLogClearCounter);

    if (!_paused && _consoleLogString.compare(log) != 0) {
        _consoleLog->setText(log);
        _consoleLog->verticalScrollBar()->setValue(
            _consoleLog->verticalScrollBar()->maximum());
        _consoleLogString = log;
    }
}

void ConsoleLogWidget::showEvent(QShowEvent *e) {
    _consoleLogTimer->start(1000);
    QWidget::showEvent(e);
}

void ConsoleLogWidget::closeEvent(QCloseEvent *e) {
    _consoleLogTimer->stop();
    QWidget::closeEvent(e);
}

void ConsoleLogWidget::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Return) {
        QList<QTextEdit::ExtraSelection> extraSelections;
        _consoleLog->moveCursor(QTextCursor::Start);

        while (_consoleLog->find(_searchBar->text())) {
            QTextEdit::ExtraSelection extra;
            extra.cursor = _consoleLog->textCursor();
            extra.format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
            extra.format.setUnderlineColor(QColor(200, 200, 200));
            extra.format.setBackground(QColor(230, 230, 230));

            extraSelections.append(extra);
        }

        _consoleLog->setExtraSelections(extraSelections);
    }
}
