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

#include  <QSqlDriver>
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


#include <ActiveQt/QAxObject>
//#include <QAxWidget>
//#include <QAxBase>


QT_BEGIN_NAMESPACE
namespace Ui {
class orcamento_ti;
}
QT_END_NAMESPACE


//---
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

    static bool enviarPeloOutlook(const EmailData &dados) {
        // Inicializa o Outlook
        QAxObject* outlook = new QAxObject("Outlook.Application", nullptr);
        if (!outlook || outlook->isNull()) {
            qDebug() << "Erro: Outlook não instalado ou falha ao iniciar.";
            return false;
        }

        // Cria o item de e-mail (0 = olMailItem)
        QAxObject* mailItem = outlook->querySubObject("CreateItem(int)", 0);
        if (!mailItem) {
            delete outlook;
            return false;
        }

        // Configura o anexo se existir
        if (!dados.caminhoAnexo.isEmpty()) {
            QAxObject* attachments = mailItem->querySubObject("Attachments");
            if (attachments) {
                attachments->dynamicCall("Add(const QString&)", QDir::toNativeSeparators(dados.caminhoAnexo));
            }
        }

        // Busca a assinatura (reaproveitando sua lógica)
        QString assinatura = buscarAssinatura(dados.vendedorId);

        // Define propriedades
        mailItem->setProperty("Subject", dados.assunto);
        mailItem->setProperty("HTMLBody", dados.corpoHtml + "<br>" + assinatura);

        // Adiciona destinatário
        QAxObject* recipients = mailItem->querySubObject("Recipients");
        if (recipients) {
            recipients->dynamicCall("Add(const QString&)", dados.destinatario);
        }

        // Exibe para o usuário revisar
        mailItem->dynamicCall("Display()");

        // Limpeza
        delete mailItem;
        delete outlook;
        return true;
    }

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

        // Aqui você chamaria sua função 'pegarAssinaturaEmail' existente
        // return pegarAssinaturaEmail(arquivos.first());
        return "--- Assinatura Padrão ---";
    }
};

//#endif // EMAILMANAGER_H

//----


class orcamento_ti : public QMainWindow
{
    Q_OBJECT

public:


    // struct SessaoUsuario {
    //     QString usuario;
    //     QString sigla;
    //     QString regiao;
    //     QString vendedor;
    //     QString uf;
    // };

    orcamento_ti(QWidget *parent = nullptr);
    ~orcamento_ti();



private slots:
    QString identificarIPs();
    void on_bt_sair_clicked();
    void atualizarTi();
    void configurarTabela() ;
    void on_bt_novo_clicked();
    void on_bt_visualizarPDF_clicked();
   QString pegarAssinaturaEmail(const QString& nome_assinatura);
    void on_actionAtividades_triggered();
    void on_actionCriar_Pedido_triggered();
    void on_bt_visualizar_clicked();
    void on_bt_ver_todos_clicked();
    void on_actionAtendente_triggered();
    void  sem_transportadora();
    void selecionarTransportadora(const QModelIndex &index);
    bool eventFilter(QObject *obj, QEvent *event) ;
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
    QSqlDatabase dba;   //permite alteração nas tabelas do Protheus

    QSqlTableModel * m_dbpModel;
    QSqlTableModel * m_dbiModel;
    QSqlTableModel * m_dbaModel;

    QTableView *tv_transportadoras;



    // --- DEFINIÇÃO DA ASSINATURA DA FUNÇÃO ---
    bool abrirConexao(QSqlDatabase &db, QString host, QString dbName, QString user, QString pass);

};
#endif // ORCAMENTO_TI_H
