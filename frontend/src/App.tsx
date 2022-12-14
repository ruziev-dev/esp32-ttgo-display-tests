import React from "react";

function App() {
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
        />
        <label htmlFor="floatingUsername">Username</label>
      </div>
      <div className="form-floating">
        <input
          type="password"
          className="form-control"
          id="floatingPassword"
          placeholder="Password"
        />
        <label htmlFor="floatingPassword">Password</label>
      </div>

      <button
        className="w-100 btn btn-lg btn-primary mt-3"
        onClick={console.log}
      >
        Sign in
      </button>
    </main>
  );
}

export default App;
