DROP TABLE IF EXISTS book_1;
CREATE TABLE book_1 (id INTEGER PRIMARY KEY AUTOINCREMENT, 
	chapter_title TEXT, body_text TEXT);


DELETE FROM books WHERE id = 1;
INSERT INTO books (id, name, table_name) VALUES (1, 'Simple Algebraic Calculator', 'book_1');

-- Book 1

INSERT INTO book_1 (chapter_title, body_text) VALUES
					('About', readfile('book_1-About.rtf'));
