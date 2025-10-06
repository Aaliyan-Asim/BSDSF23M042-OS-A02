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

### 🧾 **Feature 3 – Version 1.2.0: Column Display**

**Q1. How does the program decide how many columns and rows to use?**
The program first scans all filenames in the target directory to find the length of the longest name.
It then uses the `ioctl()` system call with `TIOCGWINSZ` to obtain the terminal width (number of columns).
Using that information, it computes:

```
col_width = max_filename_length + spacing
num_cols  = terminal_width / col_width
num_rows  = ceil(total_files / num_cols)
```

If the terminal width cannot be detected, it defaults to 80 characters.

---

**Q2. Why is `ioctl()` used, and how does it help?**
`ioctl()` (I/O control) lets a program query or configure device-specific parameters.
When used with `TIOCGWINSZ`, it returns the current terminal’s width and height, allowing `ls` to automatically adapt its output formatting to the user’s screen size.

---

**Q3. What does “down then across” mean in this context?**
Instead of printing filenames row-by-row from left to right, the program fills each column vertically first.
Example for 9 files and 3 columns:

```
file1  file4  file7
file2  file5  file8
file3  file6  file9
```

This mimics the default behavior of the real `ls` command.

---

**Q4. How is spacing and alignment handled?**
Each printed filename is left-aligned in a fixed-width column using:

```c
printf("%-*s", col_width, filenames[index]);
```

The `-*` width specifier pads filenames with spaces so columns remain aligned regardless of varying filename lengths.

---

**Q5. What dynamic-memory strategy is used?**
All filenames are stored in a dynamically allocated array (`malloc` + `realloc`), which grows as needed.
After printing, the program frees every string and the main array to prevent memory leaks.

---


