---
layout: content
title: Stack
permalink: /stack/
---

{% assign sorted_categories = site.categories | sort %}
{% for category in sorted_categories %}
{% assign cat_name = category[0] %}
{% assign icon = "" %}
{% if cat_name == "Python" %}{% assign icon = "devicon-python-plain colored" %}
{% elsif cat_name == "Pandas & Plotly" %}{% assign icon = "devicon-pandas-plain colored" %}
{% elsif cat_name == "Streamlit" %}{% assign icon = "devicon-streamlit-plain colored" %}
{% elsif cat_name == "SQL" %}{% assign icon = "devicon-azuresqldatabase-plain colored" %}
{% endif %}
{% if cat_name == "Python" %}
<details>
<summary><strong><i class="{{ icon }}"></i> {{ cat_name }}</strong> ({{ category[1] | size }})</summary>
  {% assign subcats = category[1] | map: "subcategory" | uniq | sort %}
  {% for subcat in subcats %}
  {% if subcat %}
  <details style="margin-left: 1rem;">
  <summary><strong>{{ subcat }}</strong></summary>
  <ul class="c-archives__list">
    {% for post in category[1] %}
    {% if post.subcategory == subcat %}
    <li class="c-archives__item">
      <h3>
        <a href="{{ post.url | prepend: site.baseurl }}">{{ post.title }}</a>
        <br>
        <small>{{ post.description }}</small>
      </h3>
      <p>{{ post.date | date: "%b %-d, %Y" }}</p>
    </li>
    {% endif %}
    {% endfor %}
  </ul>
  </details>
  {% endif %}
  {% endfor %}
  {% for post in category[1] %}
  {% unless post.subcategory %}
  <ul class="c-archives__list">
  <li class="c-archives__item">
    <h3>
      <a href="{{ post.url | prepend: site.baseurl }}">{{ post.title }}</a>
      <br>
      <small>{{ post.description }}</small>
    </h3>
    <p>{{ post.date | date: "%b %-d, %Y" }}</p>
  </li>
  </ul>
  {% endunless %}
  {% endfor %}
</details>
{% else %}
<details>
<summary><strong>{% if icon != "" %}<i class="{{ icon }}"></i> {% endif %}{{ cat_name }}</strong> ({{ category[1] | size }})</summary>
<ul class="c-archives__list">
  {% for post in category[1] %}
  <li class="c-archives__item">
    <h3>
      <a href="{{ post.url | prepend: site.baseurl }}">{{ post.title }}</a>
      <br>
      <small>{{ post.description }}</small>
    </h3>
    <p>{{ post.date | date: "%b %-d, %Y" }}</p>
  </li>
  {% endfor %}
</ul>
</details>
{% endif %}
{% endfor %}
