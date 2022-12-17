class Api {
  private base_url = "http://192.168.1.205/";

  private async postFetch<T>(path: string, data: any): Promise<T> {
    try {
      const response = await fetch(this.base_url + path, {
        method: "POST",
        credentials: "include",
        body: JSON.stringify(data),
      });
      if (response.ok) return await response.json();
      else throw Error(response.statusText);
    } catch (error) {
      console.log("postFetch: ", error);
      throw error;
    }
  }

  private async getFetch<T>(path: string) {
    try {
      const response = await fetch(this.base_url + path);
      if (response.ok) return await response.json();
      else throw Error(response.statusText);
    } catch (error) {
      console.log("getFetch: ", error);
      throw error;
    }
  }

  login = async (username: string, password: string) => {
    return await this.postFetch("login", { username, password });
  };

  logout = async () => {
    return await fetch(this.base_url + "logout").catch((error) =>
      console.log("logout: ", error)
    );
  };
}

export const api = new Api();
