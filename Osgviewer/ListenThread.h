#ifndef VMSENS_CONNECTION_H
#define VMSENS_CONNECTION_H


class ListenThread : public QThread
{
public:
	ListenThread(QString sIP, unsigned short uPort, void* user);

	bool m_run;

	void SetIPAndPort(QString sIP, unsigned short uPort)
	{
		if(!m_run)
		{
			m_ip = sIP;
			m_port = uPort;
		}
	}

protected:
	void run();

	bool allOk();

	QTcpSocket *m_socket;	
	QString m_ip;
	unsigned short m_port;
	void* m_user;
};

class VMSENS_ENG : public QObject
{
	Q_OBJECT

public:
	VMSENS_ENG(const char* sBVHFile = NULL, const char* sIP = "127.0.0.1", unsigned short uPort = 8889, QObject* parent = 0);
	//这个接口是为了和qavimator兼容所设计的，一般情况下可以用第一个构造函数，会自动创建Animation* animation;
	VMSENS_ENG(Animation* pAnimation, const char* sIP = "127.0.0.1", unsigned short uPort = 8889, QObject* parent = 0);
	~VMSENS_ENG();

private:

	QTcpServer *mocap_output_server;
	QTcpSocket *output_socket;
	void** mocapnodedata;
public:
	void connectToServer();
	void disconnectFromServer();

	friend class ListenThread;
	ListenThread *thread;

	private slots:
		void readSocketMessage();
		void forDisconnected();
		void comingNewConnection();
		void readyToSend();
};

#endif // VMSENS_CONNECTION_H
