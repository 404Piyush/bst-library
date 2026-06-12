/* bst.js — JavaScript port of the bst-library C API.
 *
 * Same surface as the C version:
 *     bst_create(cmp, freeKey)
 *     bst_insert(t, key, payload)
 *     bst_find(t, key)
 *     bst_remove(t, key)
 *     bst_inorder(t, visit)
 *     bst_size(t)
 *     bst_height(t)
 *
 * Used by the playground in the landing page.
 */
'use strict';

function bst_create(cmp, freeKey) {
  return { root: null, cmp, freeKey, size: 0, height: 0 };
}

function bst_insert(t, key, payload) {
  if (t.root === null) {
    t.root = { key, payload, left: null, right: null };
    t.size = 1;
    t.height = 1;
    return true;
  }
  let cur = t.root;
  for (;;) {
    const c = t.cmp(key, cur.key);
    if (c === 0) { cur.payload = payload; return false; }
    const next = c < 0 ? 'left' : 'right';
    if (cur[next] === null) {
      cur[next] = { key, payload, left: null, right: null };
      t.size++;
      t.height = Math.max(t.height, depth(t.root));
      return true;
    }
    cur = cur[next];
  }
}

function bst_find(t, key) {
  let cur = t.root;
  while (cur !== null) {
    const c = t.cmp(key, cur.key);
    if (c === 0) return cur.payload;
    cur = c < 0 ? cur.left : cur.right;
  }
  return null;
}

function bst_remove(t, key) {
  let parent = null, dir = null, cur = t.root;
  while (cur !== null && t.cmp(key, cur.key) !== 0) {
    parent = cur;
    const c = t.cmp(key, cur.key);
    dir = c < 0 ? 'left' : 'right';
    cur = cur[dir];
  }
  if (cur === null) return false;

  if (cur.left === null || cur.right === null) {
    /* Zero or one child. */
    const child = cur.left ?? cur.right;
    if (parent === null) t.root = child;
    else parent[dir] = child;
  } else {
    /* Two children: find in-order successor, splice it. */
    let sp = cur, sd = 'right', succ = cur.right;
    while (succ.left !== null) { sp = succ; sd = 'left'; succ = succ.left; }
    cur.key = succ.key; cur.payload = succ.payload;
    sp[sd] = succ.right;
  }
  t.size--;
  t.height = depth(t.root);
  return true;
}

function bst_inorder(t, visit) {
  const out = [];
  walk(t.root, visit, out);
  return out;
}
function walk(n, visit, out) {
  if (n === null) return;
  walk(n.left, visit, out);
  if (visit) visit(n);
  out.push(n.key);
  walk(n.right, visit, out);
}

function bst_size(t)   { return t.size; }
function bst_height(t) { return t.height; }
function bst_clear(t)  { t.root = null; t.size = 0; t.height = 0; }

function depth(n) {
  if (n === null) return 0;
  return 1 + Math.max(depth(n.left), depth(n.right));
}

window.bst = {
  create: bst_create, insert: bst_insert, find: bst_find,
  remove: bst_remove, inorder: bst_inorder,
  size: bst_size, height: bst_height, clear: bst_clear,
};
