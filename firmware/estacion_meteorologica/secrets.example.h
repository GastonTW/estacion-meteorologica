// ============================================================================
//  secrets.example.h  ─  PLANTILLA de credenciales
//
//  1) Copiá este archivo como  secrets.h  (en la misma carpeta).
//  2) Completá tus valores reales.
//  3) NO subas secrets.h al repo: ya está en .gitignore.
// ============================================================================
#pragma once

// ---- WiFi ------------------------------------------------------------------
#define WIFI_SSID        "TU_RED_WIFI"
#define WIFI_PASSWORD    "TU_PASSWORD"

// ---- Supabase --------------------------------------------------------------
//  URL del proyecto (sin barra final), p. ej. https://abcdxyz.supabase.co
//  La anon key es pública por diseño (protegida por RLS). Ver docs/03.
#define SUPABASE_URL       "https://TU_PROYECTO.supabase.co"
#define SUPABASE_ANON_KEY  "TU_ANON_KEY"

// ---- Telegram --------------------------------------------------------------
//  Token del bot (BotFather) y chat id destino. Ver docs/03-configuracion-nube.md
#define TELEGRAM_BOT_TOKEN "123456789:AA...tu_token..."
#define TELEGRAM_CHAT_ID   "123456789"
