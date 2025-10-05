---

# 🧾 REPORT.md

## **Feature 2 – Version 1.1.0: Complete Long Listing Format**

---

### **Q1. What is the crucial difference between the `stat()` and `lstat()` system calls? When is `lstat()` more appropriate for `ls`?**

Both `stat()` and `lstat()` are system calls used to obtain metadata about files, such as permissions, ownership, size, and timestamps.

* **`stat()`**:
  Follows symbolic links and retrieves information about the **target file** that the link points to.

* **`lstat()`**:
  Retrieves information about the **link itself**, not the target file.

✅ **In the context of `ls`:**
When implementing the `ls -l` option, we use `lstat()` because it allows us to correctly display details about symbolic links (like their name and target path) rather than the file they point to. This ensures that links are represented accurately in the output (with the leading `l` type indicator).

---

### **Q2. How does the `st_mode` field store file type and permissions, and how can bitwise operators and macros extract this information?**

The `st_mode` field in the `struct stat` structure is an integer bitmask that encodes both the **file type** and **permission bits**.

You can use **bitwise AND (`&`)** with predefined macros to check specific bits in `st_mode`.

#### 🔹 Example:

```c
if (st_mode & S_IFDIR) {
    printf("This is a directory.\n");
}

if (st_mode & S_IRUSR) {
    printf("Owner has read permission.\n");
}
```

#### 🔹 Explanation:

* `S_IFDIR`, `S_IFREG`, etc. identify the **file type** (directory, regular file, symbolic link, etc.).
* `S_IRUSR`, `S_IWUSR`, `S_IXUSR`, etc. represent **user permissions** for read, write, and execute.
* The bitwise AND operation (`&`) checks whether specific bits are set, allowing you to interpret the encoded information in `st_mode`.

---

### ✅ **Summary**

| Concept                   | Explanation                                                             |
| ------------------------- | ----------------------------------------------------------------------- |
| **`stat()` vs `lstat()`** | `stat()` follows symbolic links; `lstat()` does not.                    |
| **`st_mode` field**       | Stores both file type and permission bits.                              |
| **Bitwise check**         | Use `&` with macros like `S_IFDIR` or `S_IRUSR` to extract information. |

---
