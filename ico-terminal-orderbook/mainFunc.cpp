#include "mainFunc.h"
#include <iostream>
#include <QCommandLineParser>
#include <QString>

void SetStdinEcho(bool enable)
{
#ifdef WIN32
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hStdin, &mode);

	if (!enable)
		mode &= ~ENABLE_ECHO_INPUT;
	else
		mode |= ENABLE_ECHO_INPUT;

	SetConsoleMode(hStdin, mode);

#else
	struct termios tty;
	tcgetattr(STDIN_FILENO, &tty);
	if (!enable)
		tty.c_lflag &= ~ECHO;
	else
		tty.c_lflag |= ECHO;

	(void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

void ParserInit(QCommandLineParser &parser)
{
	parser.setApplicationDescription(QCoreApplication::translate("main", "Registers users in the database."));

	// Arguments
	parser.addPositionalArgument("host", QCoreApplication::translate("main", "The host name of the server."));
	parser.addPositionalArgument("database", QCoreApplication::translate("main", "The database name."));
	parser.addPositionalArgument("user", QCoreApplication::translate("main", "The database user on whose behalf the connection is being made."));

	// Options
	parser.addHelpOption();
	parser.addVersionOption();


	QCommandLineOption optPswd(QStringList() << "w" << "no-password",
		QCoreApplication::translate("main", "Never issue a password prompt."));
	parser.addOption(optPswd);
}

QString GetPassword(const QCommandLineParser & parser)
{
	QString password;

	if (!parser.isSet("no-password"))
	{
		std::string welcomePassword = "Enter password: ";
		std::cout << welcomePassword;

		std::string pswd;
		SetStdinEcho(false);
		std::cin >> pswd;
		SetStdinEcho(true);

		std::cout << "\r";
		std::cout.fill(' ');
		std::cout.width(strlen(welcomePassword.c_str()));
		std::cout << "\r";

		password = QString::fromStdString(pswd);
	}

	return password;
}

ConnectInfo GetConnectInfo(const QCoreApplication &app)
{
	ConnectInfo connectInfo;

	QCommandLineParser parser;
	ParserInit(parser);

	parser.process(app);

	const QStringList args = parser.positionalArguments();

	if (args.size() != 3) {
		fprintf(stderr, "%s\n", qPrintable(QCoreApplication::translate("main", "Error: Not argument.")));
		parser.showHelp(1);
	}

	connectInfo.host     = args[0];
	connectInfo.dataBase = args[1];
	connectInfo.user     = args[2];
	connectInfo.password = GetPassword(parser);

	return connectInfo;
}