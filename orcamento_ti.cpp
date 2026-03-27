#ifdef _WIN32

        if(e_outlook=="CLAS"){

            QString numOrcam = ui->tw_orcamentos->item(linhaO,0)->text();
            QString at = qrycab.value("Contato").toString();
            QString emailcli = qrycab.value("Email").toString().trimmed().toLower();
            QString assunto_email = "Kap_Orcamento_Numero_"+ui->tw_orcamentos->item(linhaO,0)->text()+"_"+data4 ;
            QString texto = "<h4>"+Hora+at+". "+"Como vai?<br><br>"
                                                        "Segue em anexo o seu orçamento: "
                                                        "Kap Orçamento Número "+numOrcam+"_"+cliente+"_"+data4+"<br><br>"
                                                                       "Ao confirmar o pedido, por gentileza, informe se o material será destinado a: <br>"
                                                                       "( ) Consumo próprio ou ativo imobilizado <br>"
                                                                       "( ) Revenda e solidário <br>"
                                                                       "( ) Industrialização <br><br>"
                                                                       "Essa informação é imprescindível para o faturamento.</h4><br><br>";

            ui->txt_msg->setText(texto);
            ui->txt_msg->selectAll();
            ui->txt_msg->copy();

            QAxObject* outlook = new QAxObject("Outlook.Application",0);
            if (!outlook) {
                qDebug() << "Falha ao iniciar o Outlook.";
                return ;
            }

            QAxObject* mailItem = outlook->querySubObject("CreateItem(int)", 0);
            if (!mailItem) {
                qDebug() << "Falha ao criar o item de e-mail.";
                delete outlook;
                return ;
            }

            QString attachmentPath = "C:/Cotacoes/"+nome;

            if (!attachmentPath.isEmpty()) {
                QAxObject* attachments = mailItem->querySubObject("Attachments");
                if (attachments) {
                    attachments->dynamicCall("Add(const QString&)", attachmentPath);
                }
            }

            QString caminhoAssinatura;
            if(e_vendedor=="000001"){
                caminhoAssinatura = QDir::homePath() +"/AppData/Roaming/Microsoft/Signatures/";
            }else{
                caminhoAssinatura = "C:/Orcamento/";
            }

            QDir dirAssinatura(caminhoAssinatura);
            QStringList filtros;
            filtros << "*.htm";
            dirAssinatura.setNameFilters(filtros);
            QStringList arquivosAssinatura = dirAssinatura.entryList(QDir::Files);
            QString nome_assinatura = arquivosAssinatura.first();
            QString assinatura = pegarAssinaturaEmail(nome_assinatura);

            mailItem->setProperty("Subject", assunto_email);
            mailItem->setProperty("HTMLBody", texto + assinatura);

            QAxObject* recipients = mailItem->querySubObject("Recipients");
            recipients->dynamicCall("Add(QString)", emailcli);

            mailItem->dynamicCall("Display()");

            delete mailItem;
            delete outlook;
        }

#else

        qDebug() << "Envio de email não suportado no Linux";

#endif