import { configure, makeAutoObservable } from "mobx";
import { api } from "../api/api";
configure({ useProxies: "never" });

class Store {
  isAuth: boolean = false;
  isLoading: boolean = false;
  errorMsg = "";
  constructor() {
    makeAutoObservable(this);
  }

  enableLoading = () => (this.isLoading = true);
  disableLoading = () => (this.isLoading = false);
  setErrorMsg(message: string) {
    this.errorMsg = message;
    setTimeout(() => {
      this.errorMsg = "";
    }, 3000);
  }

  async login(username: string, password: string) {
    this.enableLoading();

    try {
      const result = await api.login(username, password);
      console.log("store.login: ", result);
      this.isAuth = true;
    } catch (error: any) {
      this.setErrorMsg(error.message);
    }

    this.disableLoading();
  }
  async logout() {
    this.enableLoading();
    await api.logout();
    this.isAuth = false;
    this.disableLoading();
  }
}

export const store = new Store();
