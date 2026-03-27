#ifndef ORCAMENTO_TI_H
#define ORCAMENTO_TI_H

#include <QStandardItemModel>
#include <QNetworkInterface>
#include <QSqlTableModel>
#include <QHostAddress>
#include <QMessageBox>

#include <QMainWindow>
#include <QSqlDatabase>
#include <QInputDialog>
#include <QTableView>

#include <QSqlDriver>
#include <QStatusBar>
#include <QSqlQuery>
#include <QSqlError>
#include <QPainter>

#include <QDebug>
#include <QPrinter>
#include <QTime>
#include <QUuid>

#include <QDate>
#include <QFont>
#include <QFile>
#include <QSql>
#include <QUrl>
#include <QDir>

#ifdef _WIN32
#include <ActiveQt/QAxObject>
#endif

QT_BEGIN_NAMESPACE
namespace Ui {
class orcamento_ti;
}
QT_END_NAMESPACE


// ---------------- EMAIL MANAGER ----------------
class EmailManager {
public:
    struct EmailData {
        QString destinatario;
        QString assunto;
        QString corpoHtml;
        QString caminhoAnexo;
        QString vendedorId;
    };

    QString vi_t = "1";

#ifdef _WIN32
    static bool enviarPeloOutlook(const EmailData &dados) {
        QAxObject* outlook = new QAxObject("Outlook.Application", nullptr);
        if (!outlook || outlook->isNull()) {
            qDebug() << "Erro: Outlook não instalado ou falha ao iniciar.";
            return false;
        }

        QAxObject* mailItem = outlook->querySubObject("CreateItem(int)", 0);
        if (!mailItem) {
            delete outlook;
            return false;
        }

        if (!dados.caminhoAnexo.isEmpty()) {
            QAxObject* attachments = mailItem->querySubObject("Attachments");
            if (attachments) {
                attachments->dynamicCall("Add(const QString&)", QDir::toNativeSeparators(dados.caminhoAnexo));
            }
        }

        QString assinatura = buscarAssinatura(dados.vendedorId);

        mailItem->setProperty("Subject", dados.assunto);
        mailItem->setProperty("HTMLBody", dados.corpoHtml + "<br>" + assinatura);

        QAxObject* recipients = mailItem->querySubObject("Recipients");
        if (recipients) {
            recipients->dynamicCall("Add(const QString&)", dados.destinatario);
        }

        mailItem->dynamicCall("Display()");

        delete mailItem;
        delete outlook;
        return true;
    }
#else
    static bool enviarPeloOutlook(const EmailData &) {
        qDebug() << "Envio de email não suportado no Linux";
        return false;
    }
#endif

private:
    static QString buscarAssinatura(const QString &vendedorId) {
        QString caminho;
        if (vendedorId == "000001") {
            caminho = QDir::homePath() + "/AppData/Roaming/Microsoft/Signatures/";
        } else {
            caminho = "C:/Orcamento/";
        }

        QDir dir(caminho);
        QStringList arquivos = dir.entryList({"*.htm"}, QDir::Files);

        if (arquivos.isEmpty()) return "";

        return "--- Assinatura Padrão ---";
    }
};


// ---------------- MAIN CLASS ----------------
class orcamento_ti : public QMainWindow
{
    Q_OBJECT

public:
    orcamento_ti(QWidget *parent = nullptr);
    ~orcamento_ti();

private slots:
    QString identificarIPs();
    void on_bt_sair_clicked();
    void atualizarTi();
    void configurarTabela();
    void on_bt_novo_clicked();
    void on_bt_visualizarPDF_clicked();
    QString pegarAssinaturaEmail(const QString& nome_assinatura);
    void on_actionAtividades_triggered();
    void on_actionCriar_Pedido_triggered();
    void on_bt_visualizar_clicked();
    void on_bt_ver_todos_clicked();
    void on_actionAtendente_triggered();
    void sem_transportadora();
    void selecionarTransportadora(const QModelIndex &index);
    bool eventFilter(QObject *obj, QEvent *event);
    void on_bt_prospects_clicked();
    void on_bt_catalogo_clicked();
    void on_actionPedidos_triggered();
    void on_actionC_digo_do_cliente_triggered();
    void on_actionFantasia_triggered();
    void on_actionEstado_triggered();
    void on_actionProspect_triggered();
    void on_actionE_mail_do_cliente_triggered();
    void on_actionRegi_o_triggered();
    void on_actionLimpar_filtro_triggered();
    void on_actionN_mero_triggered();
    void on_actionDeletar_triggered();
    void on_actionRela_o_de_Clientes_triggered();
    void on_bt_editar_clicked();

private:
    Ui::orcamento_ti *ui;
    QSqlDatabase dbp;
    QSqlDatabase dbi;
    QSqlDatabase dba;

    QSqlTableModel * m_dbpModel;
    QSqlTableModel * m_dbiModel;
    QSqlTableModel * m_dbaModel;

    QTableView *tv_transportadoras;

    bool abrirConexao(QSqlDatabase &db, QString host, QString dbName, QString user, QString pass);
};

#endif // ORCAMENTO_TI_H