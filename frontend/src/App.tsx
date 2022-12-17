import React, { useState } from "react";
import { observer } from "mobx-react-lite";
import { store } from "./store/store";

const App = observer(() => {
  if (store.isAuth) return <SignedInStack />;
  else return <Auth />;
});

const SignedInStack = () => {
  return (
    <header className="p-3 text-bg-dark">
      <div className="container">
        <div className="d-flex flex-wrap align-items-center justify-content-center justify-content-lg-start">
          <ul className="nav col-12 col-lg-auto me-lg-auto mb-2 justify-content-center mb-md-0">
            <li>
              <a href="#" className="nav-link px-2 text-secondary">
                Home
              </a>
            </li>
            <li>
              <a href="#" className="nav-link px-2 text-white">
                Features
              </a>
            </li>
            <li>
              <a href="#" className="nav-link px-2 text-white">
                Pricing
              </a>
            </li>
            <li>
              <a href="#" className="nav-link px-2 text-white">
                FAQs
              </a>
            </li>
          </ul>

          <form
            className="col-12 col-lg-auto mb-3 mb-lg-0 me-lg-3"
            role="search"
          >
            <input
              type="search"
              className="form-control form-control-dark text-bg-dark"
              placeholder="Search..."
              aria-label="Search"
            />
          </form>

          <div className="text-end">
            <button type="button" className="btn btn-warning">
              Sign-up
            </button>

            <button
              type="button"
              className="btn btn-outline-light m-2"
              onClick={store.logout.bind(store)}
            >
              Logout
            </button>
          </div>
        </div>
      </div>
    </header>
  );
};

const Auth = observer(() => {
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");

  const onClickLogin = async () => {
    store.login(username, password);
  };

  const onChangeUsername = (event: React.ChangeEvent<HTMLInputElement>) => {
    setUsername(event.target.value?.trim());
  };
  const onChangePassword = (event: React.ChangeEvent<HTMLInputElement>) => {
    setPassword(event.target.value?.trim());
  };

  const btnDisabled = [
    store.isLoading,
    !username.trim(),
    !password.trim(),
  ].some((value) => value)
    ? "disabled"
    : "";

  return (
    <main className="form-signin w-100 m-auto">
      <img
        className="mb-3"
        src="https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32s3/_images/hw-reference.png"
        alt=""
        height="55"
      />
      <h1 className="h3 mb-3 fw-normal">Please sign in</h1>

      <div className="form-floating">
        <input
          type="username"
          className="form-control"
          id="floatingUsername"
          placeholder="Username"
          value={username}
          onChange={onChangeUsername}
        />
        <label htmlFor="floatingUsername">Username</label>
      </div>
      <div className="form-floating">
        <input
          type="password"
          className="form-control"
          id="floatingPassword"
          placeholder="Password"
          value={password}
          onChange={onChangePassword}
        />
        <label htmlFor="floatingPassword">Password</label>
      </div>

      <button
        className={`w-100 btn btn-lg btn-primary mt-3 ${btnDisabled}`}
        onClick={onClickLogin}
      >
        Sign in
      </button>
      {!!store.errorMsg && (
        <div className="h6 mt-3 text-danger">{store.errorMsg}</div>
      )}
    </main>
  );
});

export default App;
