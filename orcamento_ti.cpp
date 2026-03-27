#ifdef _WIN32

if(e_outlook=="CLAS"){

    QString numOrcam = ui->tw_orcamentos->item(linhaO,0)->text();
    QString at = qrycab.value("Contato").toString();
    QString emailcli = qrycab.value("Email").toString().trimmed().toLower();
    QString assunto_email = "Kap_Orcamento_Numero_"+ui->tw_orcamentos->item(linhaO,0)->text()+"_"+data4 ;

    QString texto = "<h4>"+Hora+at+". "+"Como vai?<br><br>"
                    "Segue em anexo o seu orçamento<br><br></h4>";

    QAxObject* outlook = new QAxObject("Outlook.Application",0);
    if (!outlook) return;

    QAxObject* mailItem = outlook->querySubObject("CreateItem(int)", 0);
    if (!mailItem){
        delete outlook;
        return;
    }

    QString attachmentPath = "C:/Cotacoes/"+nome;

    if (!attachmentPath.isEmpty()) {
        QAxObject* attachments = mailItem->querySubObject("Attachments");
        if (attachments) {
            attachments->dynamicCall("Add(const QString&)", attachmentPath);
        }
    }

    mailItem->setProperty("Subject", assunto_email);
    mailItem->setProperty("HTMLBody", texto);

    QAxObject* recipients = mailItem->querySubObject("Recipients");
    recipients->dynamicCall("Add(QString)", emailcli);

    mailItem->dynamicCall("Display()");

    delete mailItem;
    delete outlook;
}
