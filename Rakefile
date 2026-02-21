
#
## Customize post location and post extensions
#

baseurl = `cat ./_config.yml | awk '/baseurl/ { print $2 }' | sed 's/\"//g'`

SOURCE = "."
CONFIG = {
  'posts' => File.join(SOURCE, "_posts"),
  'post_ext' => "md",
  'theme_package_version' => "0.1.0"
}

#
## Color console outout support just because :D
#
module Colors
  def colorize(text, color_code)
    "\033[#{color_code}m#{text}\033[0m"
  end

  {
    :black    => 30,
    :red      => 31,
    :green    => 32,
    :yellow   => 33,
    :blue     => 34,
    :cyan     => 36,
  }.each do |key, color_code|
    define_method key do |text|
      colorize(text, color_code)
    end
  end
end
include Colors

#
## Just typing `rake` will invoke `rake preview`
#
task :default => :preview
load '_rake-configuration.rb' if File.exist?('_rake-configuration.rb')

desc 'Preview with livereload on local machine'
task :preview => :clean do
	puts green "Starting livereload server"
  jekyll('serve -L')
end
task :serve => :preview

desc 'Clean up generated site'
task :clean do
    cleanup
end

desc 'Check links for generated site'
task :check do
  STDOUT.sync = true
	cleanup
  jekyll("build -d _site#{baseurl}")
	puts cyan "Running html proofer..."
	puts `htmlproofer --assume-extension --alt-ignore '/.*/' ./_site`
end

# Usage: rake post title="A Title" [date="2012-02-09"] [tags=[tag1,tag2]] [category="category"]
desc "Begin a new post in #{CONFIG['posts']}"
task :post do
  abort("rake aborted: '#{CONFIG['posts']}' directory not found.") unless FileTest.directory?(CONFIG['posts'])
  title = ENV["title"] || "new-post"
  tags = ENV["tags"] || "[]"
  category = ENV["category"] || ""
  category = "\"#{category.gsub(/-/,' ')}\"" if !category.empty?
  slug = title.downcase.strip.gsub(' ', '-').gsub(/[^\w-]/, '')
  begin
    date = (ENV['date'] ? Time.parse(ENV['date']) : Time.now).strftime('%Y-%m-%d')
  rescue => e
    puts red "Error - date format must be YYYY-MM-DD, please check you typed it correctly!"
    exit -1
  end
  filename = File.join(CONFIG['posts'], "#{date}-#{slug}.#{CONFIG['post_ext']}")
  if File.exist?(filename)
    abort("rake aborted!") if ask("#{filename} already exists. Do you want to overwrite?", ['y', 'n']) == 'n'
  end
  
  puts cyan "Creating new post: #{filename}"
  open(filename, 'w') do |post|
    post.puts "---"
    post.puts "layout: post"
    post.puts "title: \"#{title.gsub(/-/,' ')}\""
    post.puts 'description: ""'
    post.puts "category: #{category}"
    post.puts "tags: #{tags}"
    post.puts "---"
    post.puts "{% include JB/setup %}"
  end
end # task :post

VAULT_PATH = File.expand_path("~/Library/Mobile Documents/iCloud~md~obsidian/Documents/Vault1")

# Usage: rake from_obsidian file="Project/구름 딥다이브/SQL/SQLite 3강.md" title="SQL 입문 3강" category="SQL" tags="sql,sqlite" [date="2026-02-22"] [slug="sql-3-topic"]
desc "Convert Obsidian note to blog post (strips Obsidian front matter, adds Jekyll front matter)"
task :from_obsidian do
  file = ENV["file"] || abort(red("Error: file is required. Usage: rake from_obsidian file=\"path/in/vault\""))
  title = ENV["title"] || abort(red("Error: title is required."))
  category = ENV["category"] || ""
  tags = ENV["tags"] || ""
  description = ENV["description"] || ""
  slug = ENV["slug"] || title.downcase.strip.gsub(' ', '-').gsub(/[^\w-]/, '')

  source_path = File.join(VAULT_PATH, file)
  abort(red("Error: File not found: #{source_path}")) unless File.exist?(source_path)

  begin
    date = (ENV['date'] ? Time.parse(ENV['date']) : Time.now).strftime('%Y-%m-%d')
  rescue => e
    abort(red("Error - date format must be YYYY-MM-DD"))
  end

  filename = File.join(CONFIG['posts'], "#{date}-#{slug}.#{CONFIG['post_ext']}")
  if File.exist?(filename)
    abort("rake aborted!") if ask("#{filename} already exists. Overwrite?", ['y', 'n']) == 'n'
  end

  content = File.read(source_path, encoding: 'utf-8')

  # Strip Obsidian front matter
  content = content.sub(/\A---.*?---\s*/m, '')

  # Strip Obsidian Tasks section
  content = content.sub(/\n## Tasks\n.*/m, '')

  # Strip leading "## 내용" header if present
  content = content.sub(/\A\s*## 내용\s*\n/, '')

  puts cyan "Creating post from Obsidian: #{filename}"
  open(filename, 'w') do |post|
    post.puts "---"
    post.puts "layout: post"
    post.puts "title: \"#{title}\""
    post.puts "description: \"#{description}\""
    post.puts "date: #{date}"
    post.puts "category: \"#{category}\"" unless category.empty?
    post.puts "tags: #{tags}" unless tags.empty?
    post.puts "comments: true"
    post.puts "---"
    post.puts ""
    post.puts content.strip
  end

  puts green "Done! Post created at #{filename}"
  puts yellow "Note: TIL 3섹션 구조(잘한 점/개선점/배운 점)는 직접 추가하세요."
end

#
## General support functions
#

def cleanup
  sh 'rm -rf _site'
end

def jekyll(directives = '')
  sh 'jekyll ' + directives
end

def rake_running
  `ps | grep 'rake' | grep -v 'grep' | wc -l`.to_i > 1
end

