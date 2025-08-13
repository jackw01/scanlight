<template>
  <v-app>
    <v-main>
      <Main />
    </v-main>
  </v-app>
</template>

<script>
import Main from "./components/Main.vue";
import "./assets/main.css";

export default {
  name: "App",

  components: {
    Main,
  },

  data: () => ({}),
};
</script>
