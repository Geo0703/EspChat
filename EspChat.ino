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

// --- MANTENDO AS FUNCIONALIDADES ORIGINAIS ---
String limparTexto(String s) {
  s.trim();
  s.toLowerCase();
  while (s.indexOf("  ") >= 0) { s.replace("  ", " "); }
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
      if (limparTexto(linha.substring(0, p)) == nome) {
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
      if (limparTexto(linha.substring(0, p)) != alvo) { neo.println(linha); }
    }
  }
  old.close(); neo.close();
  LittleFS.remove("/users.txt");
  LittleFS.rename("/temp.txt", "/users.txt");
}

int totalUsuarios() {
  int c = 1;
  File file = LittleFS.open("/users.txt", "r");
  if (!file) return c;
  while (file.available()) { file.readStringUntil('\n'); c++; }
  file.close();
  return c;
}

String arquivoChat(String a, String b) {
  a = limparTexto(a); b = limparTexto(b);
  return (a < b) ? "/chat_" + a + "_" + b + ".txt" : "/chat_" + b + "_" + a + ".txt";
}

// --- ALTERAÇÃO APENAS NO STYLE (DESIGN MOCKUP) ---
String topo(String titulo) {
  String h = "<html><head><meta charset='UTF-8'>";
  h += "<meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1'>";
  h += "<style>";
  // Cores Extraídas do Mockup
  h += ":root{--bg:#0b1020;--card:#171f3a;--box:#25335f;--txt:#fff;--pri:#6d5cff;--sec:#7dd3fc;--danger:#ef4444;}";
  h += ".light{--bg:#f8fafc;--card:#fff;--box:#f1f5f9;--txt:#1e293b;--pri:#6366f1;--sec:#2563eb;}";
  
  h += "body{margin:0;padding:20px;font-family:'Segoe UI',Roboto,sans-serif;background:var(--bg);color:var(--txt);transition:0.3s;}";
  h += ".top{display:flex;justify-content:space-between;align-items:center;margin-bottom:20px;}";
  h += "h2{font-size:22px;margin:0;font-weight:700;}";
  h += ".card{background:var(--card);padding:24px;border-radius:20px;box-shadow:0 10px 25px rgba(0,0,0,0.2);margin-bottom:15px;}";
  h += ".item{background:var(--box);padding:16px;border-radius:15px;margin:10px 0;display:flex;align-items:center;transition:0.2s;}";
  h += ".msg{background:var(--box);padding:12px 16px;border-radius:16px;margin:8px 0;line-height:1.4;font-size:15px;max-width:85%;}";
  
  h += "input,button{width:100%;padding:14px;border:none;border-radius:12px;margin-top:10px;font-size:16px;box-sizing:border-box;}";
  h += "input{background:var(--box);color:var(--txt);border:1px solid transparent;}";
  h += "input:focus{border-color:var(--pri);outline:none;}";
  h += "button{background:var(--pri);color:#fff;font-weight:bold;cursor:pointer;transition:opacity 0.2s;}";
  h += "button:active{opacity:0.8;}";
  
  h += "a{color:var(--pri);text-decoration:none;font-weight:600;display:inline-block;}";
  h += ".danger{background:var(--danger)!important;}";
  h += ".btn-tema{width:auto;padding:8px 12px;background:var(--box);margin:0;border-radius:10px;}";
  h += "::-webkit-scrollbar{width:5px;} ::-webkit-scrollbar-thumb{background:var(--pri);border-radius:10px;}";
  h += "</style>";

  h += "<script>";
  h += "function tema(){document.body.classList.toggle('light');localStorage.setItem('tema',document.body.classList.contains('light'));}";
  h += "window.onload=function(){if(localStorage.getItem('tema')=='true'){document.body.classList.add('light');}};";
  h += "</script>";

  h += "</head><body><div style='max-width:400px;margin:0 auto;'>";
  h += "<div class='top'><h2>" + titulo + "</h2>";
  h += "<button class='btn-tema' onclick='tema()'>🌓</button></div>";

  return h;
}

// --- SETUP E LOOP MANTIDOS ---
void setup() {
  Serial.begin(115200);
  LittleFS.begin(true);
  WiFi.softAP(ssid, password);
  MDNS.begin("espchat");

  server.on("/", []() {
    String html = topo("ESPChat");
    html += "<div style='text-align:center;margin:30px 0'><h1 style='color:var(--pri);margin:0'>ESPChat</h1><p style='opacity:0.7'>Converse na sua rede local</p></div>";
    html += "<div class='card'><form action='/login'>";
    html += "<input name='u' placeholder='Usuário' required>";
    html += "<input type='password' name='p' placeholder='Senha' required>";
    html += "<button>Entrar</button>";
    html += "</form><div style='text-align:center;margin-top:20px'><a href='/cadastro'>Não tem conta? Criar conta</a></div></div></div></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/cadastro", []() {
    String html = topo("Criar Conta");
    html += "<div class='card'><form action='/registrar'>";
    html += "<input name='u' placeholder='Novo usuário' required>";
    html += "<input name='p' placeholder='Senha' required>";
    html += "<button>Cadastrar</button>";
    html += "</form><div style='text-align:center;margin-top:20px'><a href='/'>Voltar ao Login</a></div></div></div></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/registrar", []() {
    String u = limparTexto(server.arg("u"));
    String p = server.arg("p");
    p.trim();
    if (u != "" && p != "" && !existeUsuario(u)) { salvarUsuario(u, p); }
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  });

  server.on("/login", []() {
    String u = limparTexto(server.arg("u"));
    String p = server.arg("p");
    p.trim();
    if (!loginValido(u, p)) { server.send(200, "text/html", "Login inválido"); return; }
    if (u == adminUser) server.sendHeader("Location", "/admin");
    else server.sendHeader("Location", "/usuarios?u=" + u);
    server.send(302, "text/plain", "");
  });

  server.on("/usuarios", []() {
    String user = limparTexto(server.arg("u"));
    String html = topo("Olá, " + user);
    html += "<div class='card'><a href='/geral?u=" + user + "' style='width:100%'>💬 Chat Geral</a></div>";
    html += "<h3 style='margin-left:10px;opacity:0.8'>Usuários Online</h3>";
    File file = LittleFS.open("/users.txt", "r");
    while (file.available()) {
      String linha = file.readStringUntil('\n');
      linha.trim();
      int p = linha.indexOf("|");
      if (p > 0) {
        String outro = limparTexto(linha.substring(0, p));
        if (outro != user) {
          html += "<a href='/chat?u=" + user + "&to=" + outro + "' class='item' style='display:flex;justify-content:space-between;color:var(--txt)'>";
          html += "<span>👤 " + outro + "</span><span>❯</span></a>";
        }
      }
    }
    file.close();
    html += "<div style='text-align:center;margin-top:30px'><a href='/' style='color:var(--danger)'>Sair da conta</a></div></div></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/chat", []() {
    String user = limparTexto(server.arg("u"));
    String to = limparTexto(server.arg("to"));
    String html = topo("Chat com " + to);
    html += "<div id='msgs' style='height:400px;overflow-y:auto;padding-right:5px;display:flex;flex-direction:column'></div>";
    html += "<script>function load(){fetch('/lerchat?u=" + user + "&to=" + to + "').then(r=>r.text()).then(t=>{const m=document.getElementById('msgs');m.innerHTML=t;m.scrollTop=m.scrollHeight;});}load();setInterval(load,1500);</script>";
    html += "<div class='card' style='margin-top:15px;padding:10px'><form action='/send' style='display:flex;gap:10px'>";
    html += "<input type='hidden' name='u' value='" + user + "'><input type='hidden' name='to' value='" + to + "'>";
    html += "<input name='msg' placeholder='Sua mensagem...' style='margin:0' required>";
    html += "<button style='width:auto;margin:0;padding:0 20px'>❯</button></form></div>";
    html += "<a href='/usuarios?u=" + user + "'>❮ Voltar</a></div></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/lerchat", []() {
    String u = limparTexto(server.arg("u"));
    String to = limparTexto(server.arg("to"));
    String html = "";
    File file = LittleFS.open(arquivoChat(u, to), "r");
    if (file) {
      while (file.available()) {
        String l = file.readStringUntil('\n');
        String alinhamento = (l.indexOf("<b>"+u+":</b>") != -1) ? "align-self:flex-end;background:var(--pri);color:#fff" : "align-self:flex-start";
        html += "<div class='msg' style='" + alinhamento + "'>" + l + "</div>";
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

  server.on("/geral", []() {
    String user = limparTexto(server.arg("u"));
    String html = topo("Chat Geral");
    html += "<div id='geralbox' style='height:400px;overflow-y:auto;display:flex;flex-direction:column'></div>";
    html += "<script>function loadg(){fetch('/lergeral?u=" + user + "').then(r=>r.text()).then(t=>{const m=document.getElementById('geralbox');m.innerHTML=t;m.scrollTop=m.scrollHeight;});}loadg();setInterval(loadg,1500);</script>";
    html += "<div class='card' style='margin-top:15px;padding:10px'><form action='/sendgeral' style='display:flex;gap:10px'>";
    html += "<input type='hidden' name='u' value='" + user + "'><input name='msg' placeholder='Mensagem geral...' style='margin:0' required>";
    html += "<button style='width:auto;margin:0;padding:0 20px'>❯</button></form></div>";
    html += "<a href='/usuarios?u=" + user + "'>❮ Voltar</a></div></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/lergeral", []() {
    String u = limparTexto(server.arg("u"));
    String html = "";
    File file = LittleFS.open("/geral.txt", "r");
    if (file) {
      while (file.available()) {
        String l = file.readStringUntil('\n');
        String alinhamento = (l.indexOf("<b>"+u+":</b>") != -1) ? "align-self:flex-end;background:var(--pri);color:#fff" : "align-self:flex-start";
        html += "<div class='msg' style='" + alinhamento + "'>" + l + "</div>";
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

  server.on("/admin", []() {
    String html = topo("Dashboard Admin");
    html += "<div class='card'>👥 Total de usuários: <b>" + String(totalUsuarios()) + "</b><br>🌐 IP: " + WiFi.softAPIP().toString() + "</div>";
    html += "<div class='card'><a href='/cleargeral' style='width:100%'><button class='danger'>Apagar Chat Geral</button></a></div>";
    File file = LittleFS.open("/users.txt", "r");
    while (file.available()) {
      String linha = file.readStringUntil('\n');
      int p = linha.indexOf("|");
      if (p > 0) {
        String nome = limparTexto(linha.substring(0, p));
        html += "<div class='item' style='justify-content:space-between'><span>" + nome + "</span>";
        html += "<a href='/deluser?u=" + nome + "' style='color:var(--danger)'>Apagar</a></div>";
      }
    }
    file.close();
    html += "<div style='text-align:center;margin-top:20px'><a href='/'>Sair do Admin</a></div></div></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/deluser", []() {
    String alvo = limparTexto(server.arg("u"));
    if (alvo != "admin") { apagarUsuario(alvo); }
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
