/// <reference types="vite/client" />

interface ImportMetaEnv {
  readonly VITE_CAMERA_IP: string;
}

interface ImportMeta {
  readonly env: ImportMetaEnv;
}
