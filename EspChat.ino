/*
=========================================================
ESPCHAT
ESP32 + Arduino IDE

WiFi:
EspChat
EspChaTP1

URL:
http://espchat.local
ou
http://192.168.4.1

ADMIN:
usuario: admin
senha: 123admin
=========================================================
*/

#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <ESPmDNS.h>

const char* ssid = "EspChat";
const char* password = "EspChaTP1";

WebServer server(80);

String adminUser = "admin";
String adminPass = "123admin";


String limparTexto(String s) {
  s.trim();
  s.toLowerCase();

  while (s.indexOf("  ") >= 0) {
    s.replace("  ", " ");
  }

  return s;
}

bool existeUsuario(String nome) {

  nome = limparTexto(nome);

  if (nome == adminUser) return true;

  File file = LittleFS.open("/users.txt", "r");
  if (!file) return false;

  while (file.available()) {

    String linha = file.readStringUntil('\n');
    linha.trim();

    int p = linha.indexOf("|");

    if (p > 0) {
      String u = limparTexto(linha.substring(0, p));

      if (u == nome) {
        file.close();
        return true;
      }
    }
  }

  file.close();
  return false;
}

bool loginValido(String user, String pass) {

  user = limparTexto(user);
  pass.trim();

  if (user == adminUser && pass == adminPass) return true;

  File file = LittleFS.open("/users.txt", "r");
  if (!file) return false;

  while (file.available()) {

    String linha = file.readStringUntil('\n');
    linha.trim();

    int p = linha.indexOf("|");

    if (p > 0) {

      String u = limparTexto(linha.substring(0, p));
      String s = linha.substring(p + 1);
      s.trim();

      if (u == user && s == pass) {
        file.close();
        return true;
      }
    }
  }

  file.close();
  return false;
}

void salvarUsuario(String u, String p) {

  File file = LittleFS.open("/users.txt", "a");
  file.println(limparTexto(u) + "|" + p);
  file.close();
}

void apagarUsuario(String alvo) {

  alvo = limparTexto(alvo);

  File old = LittleFS.open("/users.txt", "r");
  File neo = LittleFS.open("/temp.txt", "w");

  while (old.available()) {

    String linha = old.readStringUntil('\n');
    linha.trim();

    int p = linha.indexOf("|");

    if (p > 0) {

      String nome = limparTexto(linha.substring(0, p));

      if (nome != alvo) {
        neo.println(linha);
      }
    }
  }

  old.close();
  neo.close();

  LittleFS.remove("/users.txt");
  LittleFS.rename("/temp.txt", "/users.txt");
}

int totalUsuarios() {

  int c = 1;

  File file = LittleFS.open("/users.txt", "r");
  if (!file) return c;

  while (file.available()) {
    file.readStringUntil('\n');
    c++;
  }

  file.close();
  return c;
}

String arquivoChat(String a, String b) {

  a = limparTexto(a);
  b = limparTexto(b);

  if (a < b)
    return "/chat_" + a + "_" + b + ".txt";
  else
    return "/chat_" + b + "_" + a + ".txt";
}


String topo(String titulo) {

  String h = "";

  h += "<html><head>";
  h += "<meta charset='UTF-8'>";
  h += "<meta name='viewport' content='width=device-width,initial-scale=1'>";

  h += "<style>";

  h += ":root{--bg:#0b1020;--card:#171f3a;--box:#25335f;--txt:#fff;--pri:#6d5cff;--sec:#7dd3fc;}";
  h += ".light{--bg:#eef2ff;--card:#fff;--box:#dbeafe;--txt:#111827;--pri:#6366f1;--sec:#2563eb;}";

  h += "body{margin:0;padding:16px;font-family:Arial;background:var(--bg);color:var(--txt);}";

  h += ".top{display:flex;justify-content:space-between;align-items:center;}";

  h += ".card{background:var(--card);padding:14px;border-radius:14px;margin:10px 0;}";

  h += ".item{background:var(--box);padding:12px;border-radius:12px;margin:8px 0;}";

  h += ".msg{background:var(--box);padding:10px;border-radius:12px;margin:8px 0;}";

  h += "input,button{width:100%;padding:12px;border:none;border-radius:10px;margin-top:8px;font-size:16px;}";

  h += "input{background:var(--box);color:var(--txt);}";

  h += "button{background:var(--pri);color:#fff;font-weight:bold;}";

  h += "a{color:var(--sec);text-decoration:none;font-weight:bold;}";

  h += ".small{font-size:12px;opacity:.8;}";

  h += ".danger{background:#ef4444;}";

  h += "</style>";

  h += "<script>";
  h += "function tema(){document.body.classList.toggle('light');localStorage.setItem('tema',document.body.classList.contains('light'));}";
  h += "window.onload=function(){if(localStorage.getItem('tema')=='true'){document.body.classList.add('light');}};";
  h += "</script>";

  h += "</head><body>";

  h += "<div class='top'><h2>" + titulo + "</h2>";
  h += "<button style='width:auto;padding:8px 12px' onclick='tema()'>🌓</button></div>";

  return h;
}


void setup() {

  Serial.begin(115200);

  LittleFS.begin(true);

  WiFi.softAP(ssid, password);

  MDNS.begin("espchat");

  // HOME
  server.on("/", []() {

    String html = topo("ESPChat");

    html += "<div class='card'>";
    html += "<form action='/login'>";
    html += "<input name='u' placeholder='Usuário'>";
    html += "<input type='password' name='p' placeholder='Senha'>";
    html += "<button>Entrar</button>";
    html += "</form><br>";
    html += "<a href='/cadastro'>Criar conta</a>";
    html += "</div></body></html>";

    server.send(200, "text/html", html);
  });

  // CADASTRO
  server.on("/cadastro", []() {

    String html = topo("Criar Conta");

    html += "<div class='card'>";
    html += "<form action='/registrar'>";
    html += "<input name='u' placeholder='Novo usuário'>";
    html += "<input name='p' placeholder='Senha'>";
    html += "<button>Cadastrar</button>";
    html += "</form></div></body></html>";

    server.send(200, "text/html", html);
  });

  server.on("/registrar", []() {

    String u = limparTexto(server.arg("u"));
    String p = server.arg("p");
    p.trim();

    if (u == "" || p == "") {
      server.send(200, "text/html", "Preencha tudo");
      return;
    }

    if (existeUsuario(u)) {
      server.send(200, "text/html", "Usuário já existe");
      return;
    }

    salvarUsuario(u, p);

    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  });

  // LOGIN
  server.on("/login", []() {

    String u = limparTexto(server.arg("u"));
    String p = server.arg("p");
    p.trim();

    if (!loginValido(u, p)) {
      server.send(200, "text/html", "Login inválido");
      return;
    }

    if (u == adminUser)
      server.sendHeader("Location", "/admin");
    else
      server.sendHeader("Location", "/usuarios?u=" + u);

    server.send(302, "text/plain", "");
  });

  // LISTA
  server.on("/usuarios", []() {

    String user = limparTexto(server.arg("u"));

    String html = topo("Olá, " + user);

    html += "<div class='card'>";
    html += "<a href='/geral?u=" + user + "'>💬 Chat Geral</a>";
    html += "</div>";

    File file = LittleFS.open("/users.txt", "r");

    while (file.available()) {

      String linha = file.readStringUntil('\n');
      linha.trim();

      int p = linha.indexOf("|");

      if (p > 0) {

        String outro = limparTexto(linha.substring(0, p));

        if (outro != user) {

          html += "<div class='item'>";
          html += "<a href='/chat?u=" + user + "&to=" + outro + "'>";
          html += "👤 " + outro;
          html += "</a></div>";
        }
      }
    }

    file.close();

    html += "</body></html>";

    server.send(200, "text/html", html);
  });

  // CHAT PRIVADO
  server.on("/chat", []() {

    String user = limparTexto(server.arg("u"));
    String to   = limparTexto(server.arg("to"));

    String html = topo("Chat com " + to);

    html += "<div id='msgs' class='card'></div>";

    html += "<script>";
    html += "function load(){fetch('/lerchat?u=" + user + "&to=" + to + "')"
            ".then(r=>r.text()).then(t=>document.getElementById('msgs').innerHTML=t);}";
    html += "load();setInterval(load,1500);";
    html += "</script>";

    html += "<div class='card'>";
    html += "<form action='/send'>";
    html += "<input type='hidden' name='u' value='" + user + "'>";
    html += "<input type='hidden' name='to' value='" + to + "'>";
    html += "<input name='msg' placeholder='Mensagem'>";
    html += "<button>Enviar</button>";
    html += "</form></div>";

    html += "<a href='/usuarios?u=" + user + "'>Voltar</a>";

    html += "</body></html>";

    server.send(200, "text/html", html);
  });

  server.on("/lerchat", []() {

    String u = limparTexto(server.arg("u"));
    String to = limparTexto(server.arg("to"));

    String html = "";

    File file = LittleFS.open(arquivoChat(u, to), "r");

    if (file) {
      while (file.available()) {
        html += "<div class='msg'>";
        html += file.readStringUntil('\n');
        html += "</div>";
      }
      file.close();
    }

    server.send(200, "text/html", html);
  });

  server.on("/send", []() {

    String u = limparTexto(server.arg("u"));
    String to = limparTexto(server.arg("to"));
    String msg = server.arg("msg");

    File file = LittleFS.open(arquivoChat(u, to), "a");
    file.println("<b>" + u + ":</b> " + msg);
    file.close();

    server.sendHeader("Location", "/chat?u=" + u + "&to=" + to);
    server.send(302, "text/plain", "");
  });

  // CHAT GERAL
  server.on("/geral", []() {

    String user = limparTexto(server.arg("u"));

    String html = topo("Chat Geral");

    html += "<div id='geralbox' class='card'></div>";

    html += "<script>";
    html += "function loadg(){fetch('/lergeral')"
            ".then(r=>r.text()).then(t=>document.getElementById('geralbox').innerHTML=t);}";
    html += "loadg();setInterval(loadg,1500);";
    html += "</script>";

    html += "<div class='card'>";
    html += "<form action='/sendgeral'>";
    html += "<input type='hidden' name='u' value='" + user + "'>";
    html += "<input name='msg' placeholder='Mensagem geral'>";
    html += "<button>Enviar</button>";
    html += "</form></div>";

    html += "<a href='/usuarios?u=" + user + "'>Voltar</a>";

    html += "</body></html>";

    server.send(200, "text/html", html);
  });

  server.on("/lergeral", []() {

    String html = "";

    File file = LittleFS.open("/geral.txt", "r");

    if (file) {
      while (file.available()) {
        html += "<div class='msg'>";
        html += file.readStringUntil('\n');
        html += "</div>";
      }
      file.close();
    }

    server.send(200, "text/html", html);
  });

  server.on("/sendgeral", []() {

    String u = limparTexto(server.arg("u"));
    String msg = server.arg("msg");

    File file = LittleFS.open("/geral.txt", "a");
    file.println("<b>" + u + ":</b> " + msg);
    file.close();

    server.sendHeader("Location", "/geral?u=" + u);
    server.send(302, "text/plain", "");
  });

  // ADMIN
  server.on("/admin", []() {

    String html = topo("Dashboard Admin");

    html += "<div class='card'>";
    html += "👥 Total usuários: " + String(totalUsuarios()) + "<br>";
    html += "🌐 URL: espchat.local";
    html += "</div>";

    html += "<div class='card'>";
    html += "<a href='/cleargeral'><button>Apagar Chat Geral</button></a>";
    html += "</div>";

    File file = LittleFS.open("/users.txt", "r");

    while (file.available()) {

      String linha = file.readStringUntil('\n');
      linha.trim();

      int p = linha.indexOf("|");

      if (p > 0) {

        String nome = limparTexto(linha.substring(0, p));

        html += "<div class='item'>";
        html += nome;
        html += "<br><br>";
        html += "<a href='/deluser?u=" + nome + "'>";
        html += "<button class='danger'>Apagar usuário</button>";
        html += "</a></div>";
      }
    }

    file.close();

    html += "</body></html>";

    server.send(200, "text/html", html);
  });

  server.on("/deluser", []() {

    String alvo = limparTexto(server.arg("u"));

    if (alvo != "admin") {
      apagarUsuario(alvo);
    }

    server.sendHeader("Location", "/admin");
    server.send(302, "text/plain", "");
  });

  server.on("/cleargeral", []() {

    LittleFS.remove("/geral.txt");

    server.sendHeader("Location", "/admin");
    server.send(302, "text/plain", "");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
