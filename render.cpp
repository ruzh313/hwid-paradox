void render::round_rect(int x, int y, int w, int h, int r, Color color) {
    Vertex round[64];

    for (int i = 0; i < 4; i++) {
        int _x = x + ((i < 2) ? (w - r) : r);
        int _y = y + ((i % 3) ? (h - r) : r);

        float a = 90.f * i;

        for (int j = 0; j < 16; j++) {
            float _a = math::deg_to_rad(a + j * 6.f);

            round[(i * 16) + j] = Vertex(vec2_t(_x + r * sin(_a), _y - r * cos(_a)));
        }
    }

    g_csgo.m_surface->DrawSetColor(color);
    g_csgo.m_surface->DrawTexturedPolygon(64, round);
}
