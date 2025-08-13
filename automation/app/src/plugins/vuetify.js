// Styles
import "@mdi/font/css/materialdesignicons.css";
import "vuetify/styles";

// Vuetify
import { createVuetify } from "vuetify";
import colors from "vuetify/lib/util/colors";

export default createVuetify({
  theme: {
    defaultTheme: "light",
    themes: {
      light: {
        dark: false,
        colors: {
          background: colors.grey.lighten3,
          primary: colors.grey.darken1,
          secondary: colors.indigo.base,
          accent: colors.indigo.base,
          error: colors.deepOrange.accent4,
        },
      },
      dark: {
        dark: false,
      },
    },
  },
});
