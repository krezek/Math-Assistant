DROP TABLE IF EXISTS book_2;
CREATE TABLE book_2 (id INTEGER PRIMARY KEY AUTOINCREMENT, 
	chapter_title TEXT, body_text TEXT);


DELETE FROM books WHERE id = 2;
INSERT INTO books (id, name, table_name) VALUES (2, 'Elementare Arithmetik und Algebra', 'book_2');

-- Book 1

INSERT INTO book_2 (chapter_title, body_text) VALUES
					('Vorwort', readfile('book_2-Vorwort.rtf'));
INSERT INTO book_2 (chapter_title, body_text) VALUES
					('Inhaltsverzeichnis', readfile('book_2-Inhaltsverzeichnis.rtf'));
INSERT INTO book_2 (chapter_title, body_text) VALUES
					('Die arithmetische Kurzschrift', readfile('book_2-Die arithmetische Kurzschrift.rtf'));
