# 📡 ESPChat

Sistema de chat local desenvolvido para **ESP32**, funcionando como servidor Wi-Fi próprio, onde celulares, tablets ou notebooks se conectam diretamente ao ESP e conversam entre si pelo navegador.

> Sem internet • Sem app instalado • Sem servidor externo

---

# 🚀 Visão Geral

O **ESPChat** transforma um ESP32 em uma mini central de mensagens portátil.

Ao ligar o dispositivo:

* O ESP32 cria uma rede Wi-Fi própria
* Usuários se conectam nela
* Acessam `http://espchat.local`
* Criam conta ou fazem login
* Conversam em tempo real

---

# ✨ Funcionalidades

## 👥 Usuários

* Cadastro de contas
* Login por usuário e senha
* Correção automática de espaços no nome
* Nome padronizado em minúsculo

## 💬 Chat

### Privado

* Conversa entre usuários
* Histórico salvo no ESP32
* Atualização automática sem recarregar página

### Geral

* Sala pública para todos os usuários
* Mensagens em tempo real

## 🎨 Interface
![Mockup Espchat](https://drive.google.com/file/d/11tBtJeZ7pZ_SVzMvjpcWQtTgrWTbknlG/view?usp=drivesdk)
* Tema escuro
  n- Tema claro
* Visual responsivo para celular
* Interface leve

## 🔐 Admin

Painel administrativo exclusivo:

* Visualizar total de usuários
* Apagar usuários
* Limpar chat geral

---

# 📶 Como Funciona

```text
ESP32 ligado
   ↓
Cria Wi-Fi: EspChat
   ↓
Usuário conecta
   ↓
Abre navegador
   ↓
http://espchat.local
   ↓
Usa o sistema
```

---

# 🧠 Tecnologias Utilizadas

* ESP32
* Arduino IDE
* WiFi.h
* WebServer.h
* LittleFS
* ESPmDNS

---

# 📂 Estrutura de Dados

```text
/users.txt            -> usuários cadastrados
/geral.txt            -> chat geral
/chat_user1_user2.txt -> chats privados
```

---

# 🔑 Credenciais Padrão

## Wi-Fi

```text
SSID: EspChat
Senha: EspChaTP1
```

## Admin

```text
Usuário: admin
Senha: 123admin
```

---

# 🌐 Acesso

Após conectar no Wi-Fi do ESP32:

```text
http://espchat.local
```

ou

```text
http://192.168.4.1
```

---

# 📦 Instalação

1. Conecte ESP32 via USB
2. Abra o projeto na Arduino IDE
3. Selecione a placa correta
4. Faça upload do código
5. Reinicie o ESP32

---

# ⚡ Performance Real

| Modelo      | Usuários simultâneos |
| ----------- | -------------------- |
| ESP32 comum | 5 ~ 10               |
| ESP32 WROOM | 10 ~ 20              |
| ESP32 S3    | 20+                  |

---

# 🛠 Futuras Melhorias

* Envio de áudio
* Upload de imagens
* Notificações push
* Criptografia de mensagens
* Multi ESP32 (repetidores)
* App Android/WebView
* PWA instalável
* Sistema de grupos

---

# ❤️ Projeto Maker

Esse projeto mostra até onde um ESP32 pode ir:

> Um microcontrolador servindo um sistema social real.

---

# 📜 Licença

Livre para estudos, melhorias e uso pessoal.

---

# 👨‍💻 Autor
Geovane Augusto
