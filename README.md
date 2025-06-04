---

## MP_ChatSystem - Multiplayer Chat System Plugin for Unreal Engine

MP_ChatSystem is a powerful, efficient, and modular multiplayer chat system built for Unreal Engine.
Supports Global, Team, Party, and Private (DM) channels with a room-based design and highly optimized message replication — ideal for both Listen Server and Dedicated Server projects, it's Scalable, extendable, and open-source.

---

## 🚀 Features

* 💬 Structured support for **Global**, **Team**, **Party**, and **Private (DM)** chat
* 🧩 Room-based system: clean isolation of conversations and channels
* 📡 Network-efficient via **FastArray Replication** — not RPC per message
* 🔀 Supports **temporary** and **persistent** rooms per use case
* 🧠 Client-specific **relevancy filtering** — only receive what you need
* 🎛️ Full runtime control: create, join, route messages through code or UI
* 🖼️ Built-in **scrollable chat box system** for easy UI integration
* ⚙️ Easy to extend, debug, and customize for games, simulation, and more

---

## 🧩 Plugin Code Features

### 📁 Core Classes

| Class                         | Role                                                                                  |
| ----------------------------- | ------------------------------------------------------------------------------------- |
| `AMP_ChatManager`             | Central server-side manager that handles room creation, joining, and message routing. |
| `AMP_ChatRoom`                | Replicated actor that holds message state per room. Sent only to relevant clients.    |
| `UMP_ChatBox`                 | ScrollBox class for displaying dynamic chat messages using slot injection.            |
| `IMP_Chat_PlayerController_I` | Interface to fetch player identity like SenderID/Name from any PlayerController.      |

---

### 🧱 Structs & Enums

#### 📦 Structs

| Struct             | Purpose                                                           |
| ------------------ | ----------------------------------------------------------------- |
| `FMP_ChatMessage`  | Stores chat message data: sender, body, timestamp, channel, etc.  |
| `FMP_ChatMsgItem`  | Wraps a message in FastArray for replication tracking.            |
| `FMP_ChatMsgArray` | The actual `FFastArraySerializer`, replicates all message deltas. |
| `FMP_ChatRoomKey`  | Unique identifier for a room (used internally).                   |
| `FMP_ChatRoomDesc` | Room metadata (optional name, visibility, etc.).                  |

#### 📘 Enums

| Enum           | Description                                                            |
| -------------- | ---------------------------------------------------------------------- |
| `EChatChannel` | Channel types: `Global`, `Team`, `Party`, `Private`, `System`, `Admin` |

---

## 🧠 Architecture & Replication Model

* Each chat room is one replicated actor (`AMP_ChatRoom`), spawned on the server.
* Clients receive that actor **only if they've joined the room**.
* This filtering is handled using `IsNetRelevantFor()` to decide which clients should see the actor.
* Global rooms can be marked as `bAlwaysRelevant = true` to make them visible to all.

> This design ensures minimal bandwidth, no duplicated actors, and room-specific visibility — scalable and efficient.

---

## 🛫 Getting Started

### 📥 Installation

```bash
git clone https://github.com/Yuvi-GD/MP_ChatSystem.git
```

1. Copy the `MP_ChatSystem` folder into your project's `Plugins/` directory.
2. Open your project → **Edit ➡ Plugins ➡ Enable "MP\_ChatSystem"**
3. Restart Unreal Editor when prompted.

---

### 🛠 Usage

1. **Place `AMP_ChatManager`** in your level (or spawn it via GameMode).
2. In your PlayerController Blueprint, **implement `IMP_Chat_PlayerController_I`**

   * Define `GetSenderID()` and `GetSenderName()`.
3. Add `UMP_ChatBox` to your widget hierarchy.
4. On input: call a Server RPC → Manager → Room → `AddChatMessage(...)`
5. Bind `AMP_ChatRoom::OnMessageReceived` to a Widget function for UI updates.

---

## 🧪 Replication Flow

1. Client sends message → RPC → `AMP_ChatManager`
2. Manager finds correct `AMP_ChatRoom`
3. Server calls `Room->AddChatMessage(...)`
4. Message is added to `FMP_ChatMsgArray`
5. `MarkItemDirty(...)` triggers FastArray delta replication
6. Client receives `OnRep_Messages()` and dispatches to UI

---

## 🧼 Best Practices

* Use `IsNetRelevantFor()` to control who receives which room
* Use `bAlwaysRelevant = true` for global chat
* Use `ForceNetUpdate()` and `FlushNetDormancy()` after join/subscription
* Never spawn room actors on clients manually — server handles replication
* Avoid calling expensive logic (e.g., Blueprint interfaces) inside `Tick` or `IsNetRelevantFor()` if not needed

---

## 🤝 Contributing  
We welcome all contributions to help improve and expand **MP_ChatSystem**! Whether it's **bug fixes, feature enhancements, documentation improvements, or performance optimizations**, we'd love to have your input.  

### **How to Contribute**  
To keep all improvements within this main repository and prevent fragmented forks, follow these steps:  

1️⃣ **Clone the repository** → `git clone https://github.com/Yuvi-GD/MP_ChatSystem.git`  
2️⃣ **Create a new branch** → `git checkout -b feature-name`  
3️⃣ **Make your changes and commit** → `git commit -m "Description of changes"`  
4️⃣ **Push to the main repo** → `git push origin feature-name`  
5️⃣ **Open a Pull Request (PR)** in this repository.  

### **Guidelines for Contributors**  
✔ **Work inside this repo** instead of forking—our goal is collaborative development.  
✔ **Use feature branches** for all new changes to keep `main` stable.  
✔ **Discuss major changes in Issues or Discussions** before making PRs.  
✔ **Write clear commit messages** and document your changes properly.  

---

## 📜 License

Licensed under the **GNU General Public License v3.0**.

You may use, modify, and distribute this plugin under the license terms.
See the [LICENSE](./LICENSE) file for details.

---

## 💬 Feedback & Suggestions

We'd love to hear from you!

* 💡 Found a bug?
* Have an idea for improvement?
* Want to request a new feature? 
* Want a new channel type?
* Need better save/persist logic?
* Thinking about database sync or REST API features?
* Would you like to add any additional sections or details to the README file?  

Feel free to open an **[Issue](https://github.com/Yuvi-GD/MP_ChatSystem/issues)** or Discussion to share your thoughts!
let's build it together.

---

