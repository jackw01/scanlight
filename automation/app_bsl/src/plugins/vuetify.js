// Styles
import "@mdi/font/css/materialdesignicons.css";
import "vuetify/styles";

// Vuetify
import { createVuetify } from "vuetify";
import colors from "vuetify/lib/util/colors";

export default createVuetify({
  theme: {
    defaultTheme: "system",
    themes: {
      light: {
        dark: false,
        colors: {
          background: colors.grey.lighten3,
          primary: colors.grey.darken1,
          secondary: colors.teal.base,
          accent: colors.teal.base,
          error: colors.deepOrange.accent4,
        },
      },
      dark: {
        dark: true,
        colors: {
          primary: colors.grey.darken3,
          secondary: colors.teal.base,
          accent: colors.teal.base,
          error: colors.deepOrange.accent4,
        },
      },
    },
  },
});
